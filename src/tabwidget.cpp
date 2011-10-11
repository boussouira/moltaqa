#include "tabwidget.h"
#include <qevent.h>
#include <qmenu.h>
#include <qaction.h>
#include "librarybook.h"
#include "richbookreader.h"
#include "webview.h"

TabWidget::TabWidget(QWidget *parent) : QTabWidget(parent)
{
    m_tabBar = new QTabBar(this);

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
    int index = addTab(book, book->dbHandler()->bookInfo()->bookDisplayName);
    return index;
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
    QAction *moveAct = new QAction(tr("نقل الى نافذة اخرى"), &menu);
    QAction *revAct = new QAction(tr("عكس تجاور النوافذ"), &menu);
    menu.addAction(moveAct);
    menu.addAction(revAct);

    QAction *ret = menu.exec(QCursor::pos());
    if(ret) {
        if(ret == moveAct) {
            int tabIndex = m_tabBar->tabAt(point);
            if(tabIndex != -1)
                emit moveToOtherTab(tabIndex);
        } else if(ret == revAct) {
            emit reverseSplitter();
        }
    }
}
