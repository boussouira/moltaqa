#include "tabwidget.h"
#include <qevent.h>
#include <qmenu.h>
#include <qaction.h>
#include "librarybook.h"
#include "richbookreader.h"
#include "webview.h"
#include "utils.h"

TabWidget::TabWidget(QWidget *parent) : QTabWidget(parent)
{
    m_tabBar = new QTabBar(this);
    m_canMoveToOtherTabWidget = true;

    m_tabBar->setTabsClosable(true);
    m_tabBar->setContextMenuPolicy(Qt::CustomContextMenu);

    setTabBar(m_tabBar);
    setMovable(true);

    installEventFilter(this);
    m_tabBar->installEventFilter(this);

    connect(m_tabBar, SIGNAL(tabMoved(int, int)), this, SIGNAL(tabMoved(int,int)));
    connect(m_tabBar, SIGNAL(customContextMenuRequested(QPoint)), SLOT(showTabBarMenu(QPoint)));
}


int TabWidget::addBookWidget(BookWidget *book)
{
    QString bookName = book->bookReader()->bookInfo()->bookDisplayName;
    QString shortBookName = Utils::abbreviate(bookName, 30);

    int index = addTab(book, shortBookName);
    setTabToolTip(index, bookName);

    connect(book, SIGNAL(gotFocus()), SIGNAL(gotFocus()));

    return index;
}

void TabWidget::setCanMoveToOtherTabWidget(bool canMove)
{
    m_canMoveToOtherTabWidget = canMove;
}

bool TabWidget::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);

    if(event->type() == QEvent::KeyPress
            || event->type() == QEvent::FocusIn
            || event->type() == QEvent::MouseButtonPress) {
        if(count())
            emit gotFocus();
    }

    return false;
}

void TabWidget::showTabBarMenu(QPoint point)
{
    QMenu menu(this);

    QAction *closeAct = 0;
    QAction *closeOtherAct = 0;
    QAction *closeAllAct =0;

    QAction *moveAct = 0;
    QAction *revAct = 0;

    if(tabsClosable()) {
        QAction *closeAct = new QAction(tr("اغلاق التبويب"), &menu);
        QAction *closeOtherAct = new QAction(tr("اغلاق كل التبويبات الاخرى"), &menu);
        QAction *closeAllAct = new QAction(tr("اغلاق كل التبويبات"), &menu);

        menu.addAction(closeAct);
        menu.addAction(closeOtherAct);
        menu.addAction(closeAllAct);
    }

    if(m_canMoveToOtherTabWidget) {
        moveAct = new QAction(tr("نقل الى نافذة اخرى"), &menu);
        revAct = new QAction(tr("عكس تجاور النوافذ"), &menu);

        menu.addSeparator();
        menu.addAction(moveAct);
        menu.addAction(revAct);
    }

    QAction *ret = menu.exec(QCursor::pos());
    if(ret) {
        if(ret == moveAct) {
            int tabIndex = m_tabBar->tabAt(point);
            if(tabIndex != -1)
                emit moveToOtherTab(tabIndex);
        } else if(ret == revAct) {
            emit reverseSplitter();
        } else if(ret == closeAct) {
            int tabIndex = m_tabBar->tabAt(point);
            if(tabIndex != -1)
                emit tabCloseRequested(tabIndex);
        } else if(ret == closeOtherAct) {
            QWidget *w = widget(m_tabBar->tabAt(point));
            if(w) {
                for(int i=count()-1; i>=0; i--) {
                    if(widget(i) != w) {
                        emit tabCloseRequested(i);
                    }
                }
            }
        } else if(ret == closeAllAct) {
            for(int i=count()-1; i>=0; i--) {
                emit tabCloseRequested(i);
            }
        }
    }
}
