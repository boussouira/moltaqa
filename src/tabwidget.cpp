#include "tabwidget.h"

TabWidget::TabWidget(QWidget *parent) : QTabWidget(parent), m_tab(new QTabBar(this))
{
    m_tab->setTabsClosable(true);
    setTabBar(m_tab);
    setMovable(true);

    connect(m_tab, SIGNAL(tabMoved(int, int)), this, SLOT(tabIsMoved(int,int)));
    connect(m_tab, SIGNAL(currentChanged(int)), this, SLOT(updateTabBar()));
    connect(m_tab, SIGNAL(tabCloseRequested(int)), this, SLOT(updateTabBar()));
}

void TabWidget::tabIsMoved(int from, int to)
{
    emit tabMoved(from, to);
}

void TabWidget::updateTabBar()
{
    if(m_tab->count() > 1)
        m_tab->setTabsClosable(true);
    else
        m_tab->setTabsClosable(false);
}
