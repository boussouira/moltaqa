#include "ktab.h"
#include "constant.h"
#include "pageinfo.h"

KTab::KTab(QWidget *parent) : QTabWidget(parent), m_tab(new QTabBar(this))
{
    this->setTabsClosable(false);
    this->setTabBar(m_tab);
    this->setMovable(true);

    connect(m_tab, SIGNAL(tabMoved(int, int)), this, SLOT(tabIsMoved(int,int)));

}

QWidget *KTab::newOnglet()
{

    QWidget *pageOnglet = new QWidget;
    QWebView *pageWeb = new QWebView;

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(pageWeb);
    pageOnglet->setLayout(layout);

    PageInfo *pageInfo = new PageInfo();
    m_sowarInfo.append(pageInfo);

    return pageOnglet;
}

void KTab::addNewOnglet()
{
    this->addTab(newOnglet(), QURAN);
    this->setCurrentIndex(this->count()-1);

    if(this->count() > 1)
        this->setTabsClosable(true);
}

QWebView *KTab::currentPage()
{
    return this->currentWidget()->findChild<QWebView *>();
}

PageInfo *KTab::currentPageInfo()
{
    return m_sowarInfo.value(this->currentIndex());
}

void KTab::closeTab(int tabIndex)
{
    this->removeTab(tabIndex);
    m_sowarInfo.removeAt(tabIndex);
    emit reloadCurrentPageInfo();

    // Let's make sure that the last tab well never get closed!
    if(this->count() == 1)
        this->setTabsClosable(false);
}
void KTab::tabIsMoved(int from, int to)
{
    m_sowarInfo.move(from, to);
    emit tabMoved(from, to);
}
