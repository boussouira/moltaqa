#include "ktab.h"

KTab::KTab(QWidget *parent) : QTabWidget(parent), m_tab(new QTabBar(this))
{
    m_tab->setTabsClosable(true);
    setTabBar(m_tab);
    setMovable(true);

    connect(m_tab, SIGNAL(tabMoved(int, int)), this, SLOT(tabIsMoved(int,int)));
}

void KTab::tabIsMoved(int from, int to)
{
    emit tabMoved(from, to);
}

void KTab::updateTabBar()
{
    if(m_tab->count() > 1)
        m_tab->setTabsClosable(true);
    else
        m_tab->setTabsClosable(false);
}
