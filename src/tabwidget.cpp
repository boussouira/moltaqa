#include "tabwidget.h"

TabWidget::TabWidget(QWidget *parent) : QTabWidget(parent)
{
    m_tabBar = new QTabBar(this);

    m_tabBar->setTabsClosable(true);
    setTabBar(m_tabBar);
    setMovable(true);

    connect(m_tabBar, SIGNAL(tabMoved(int, int)), this, SIGNAL(tabMoved(int,int)));
    connect(m_tabBar, SIGNAL(currentChanged(int)), this, SLOT(updateTabBar()));
    connect(m_tabBar, SIGNAL(tabCloseRequested(int)), this, SLOT(updateTabBar()));
}

void TabWidget::updateTabBar()
{
    if(m_tabBar->count() > 1)
        m_tabBar->setTabsClosable(true);
    else
        m_tabBar->setTabsClosable(false);
}
