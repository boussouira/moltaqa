#include "ktab.h"

KTab::KTab(QWidget *parent) : QTabWidget(parent), m_tab(new QTabBar(this))
{
    setTabsClosable(false);
    setTabBar(m_tab);
    setMovable(true);

    connect(m_tab, SIGNAL(tabMoved(int, int)), this, SLOT(tabIsMoved(int,int)));

}

QWidget *KTab::newOnglet(Page::Type pPageType)
{

    QWidget *pageOnglet = new QWidget;
    QWebView *pageWeb = new QWebView;

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(pageWeb);
    pageOnglet->setLayout(layout);

    PageInfo *pageInfo = new PageInfo();
    pageInfo->setPageType(pPageType);
    m_sowarInfo.append(pageInfo);

    return pageOnglet;
}

void KTab::addNewOnglet(Page::Type pPageType)
{
    int newTabIndex = addTab(newOnglet(pPageType), trUtf8("القرآن الكريم"));
    setCurrentIndex(newTabIndex);

    if(count() > 1)
        setTabsClosable(true);
}

QWebView *KTab::currentPage()
{
    return currentWidget()->findChild<QWebView *>();
}

PageInfo *KTab::currentPageInfo()
{
    return m_sowarInfo.value(currentIndex());
}

void KTab::closeTab(int tabIndex)
{
    removeTab(tabIndex);
    m_sowarInfo.removeAt(tabIndex);
    emit reloadCurrentPageInfo();

    // Let's make sure that the last tab well never get closed!
    if(count() == 1)
        setTabsClosable(false);
}
void KTab::tabIsMoved(int from, int to)
{
    m_sowarInfo.move(from, to);
    emit tabMoved(from, to);
}
