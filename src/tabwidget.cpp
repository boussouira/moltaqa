#include "tabwidget.h"
#include <qevent.h>
#include <qmenu.h>
#include <qaction.h>
#include "librarybook.h"
#include "richbookreader.h"
#include "webview.h"
#include "utils.h"
#include "stringutils.h"

TabWidget::TabWidget(QWidget *parent) : QTabWidget(parent)
{
    m_tabBar = new QTabBar(this);
    m_canMoveToOtherTabWidget = false;
    m_autoClose = false;
    m_closeLastTab = true;

    setTabClosable();
    m_tabBar->setContextMenuPolicy(Qt::CustomContextMenu);

    setTabBar(m_tabBar);
    setMovable(true);

    installEventFilter(this);
    m_tabBar->installEventFilter(this);

    connect(m_tabBar, SIGNAL(tabMoved(int, int)), this, SIGNAL(tabMoved(int,int)));
    connect(m_tabBar, SIGNAL(customContextMenuRequested(QPoint)), SLOT(showTabBarMenu(QPoint)));
    connect(this, SIGNAL(tabCloseRequested(int)), SLOT(closeTab(int)));
    connect(this, SIGNAL(currentChanged(int)), SLOT(setTabClosable()));
}


int TabWidget::addBookWidget(BookWidget *book)
{
    QString bookName = book->bookReader()->bookInfo()->title;
    QString shortBookName = Utils::String::abbreviate(bookName, 30);

    int index = addTab(book, shortBookName);
    setTabToolTip(index, bookName);

    connect(book, SIGNAL(gotFocus()), SIGNAL(gotFocus()));

    return index;
}

void TabWidget::setCanMoveToOtherTabWidget(bool canMove)
{
    m_canMoveToOtherTabWidget = canMove;
}

void TabWidget::setEnableTabBar(bool enable)
{
    m_tabBar->setEnabled(enable);
}

void TabWidget::setAutoTabClose(bool autoClose)
{
    m_autoClose = autoClose;
}

void TabWidget::setCloseLastTab(bool closeLast)
{
    m_closeLastTab = closeLast;
}

void TabWidget::setTabBarActions(QList<QAction *> list)
{
    m_tabBarActions = list;
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

    if(tabsClosable()) {
        closeAct = new QAction(tr("اغلاق التبويب"), &menu);
        closeOtherAct = new QAction(tr("اغلاق كل التبويبات الاخرى"), &menu);
        closeAllAct = new QAction(tr("اغلاق كل التبويبات"), &menu);

        menu.addAction(closeAct);
        menu.addAction(closeOtherAct);
        menu.addAction(closeAllAct);
    }

    if(m_tabBarActions.size()) {
        menu.addSeparator();

        foreach (QAction *act, m_tabBarActions) {
            if(act)
                menu.addAction(act);
            else
                menu.addSeparator();
        }
    }

    int tabIndex = m_tabBar->tabAt(point);
    if(tabIndex != -1)
        setCurrentIndex(tabIndex);

    QAction *ret = menu.exec(QCursor::pos());
    if(ret) {
        if(ret == closeAct) {
            if(tabIndex != -1)
                emit tabCloseRequested(tabIndex);
        } else if(ret == closeOtherAct) {
            QWidget *w = widget(tabIndex);
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

void TabWidget::closeTab(int index)
{
    ml_return_on_fail(m_autoClose && (m_closeLastTab || count() > 1));

    QWidget *w = widget(index);
    ml_return_on_fail(w);

    removeTab(index);
    delete w;

    if(!count())
        emit lastTabClosed();
}

void TabWidget::setTabClosable()
{
    m_tabBar->setTabsClosable(m_closeLastTab || count() > 1);
}
