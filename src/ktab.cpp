#include "ktab.h"

KTab::KTab(QWidget *parent) : QTabWidget(parent), m_tab(new QTabBar(this))
{
    setTabsClosable(false);
    setTabBar(m_tab);
    setMovable(true);
    setDocumentMode(true);

    connect(m_tab, SIGNAL(tabMoved(int, int)), this, SLOT(tabIsMoved(int,int)));

}

void KTab::closeTab(int tabIndex)
{
    removeTab(tabIndex);
    emit reloadCurrentPageInfo();

    // Let's make sure that the last tab well never get closed!
    if(count() == 1)
        setTabsClosable(false);
}
void KTab::tabIsMoved(int from, int to)
{
    emit tabMoved(from, to);
}
