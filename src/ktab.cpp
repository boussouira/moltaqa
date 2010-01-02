#include "ktab.h"

KTab::KTab(QWidget *parent) : QTabWidget(parent)
{
    this->setTabsClosable(false);
    this->setMovable(true);
}

QWidget *KTab::newOnglet()
{

    QWidget *pageOnglet = new QWidget;
    QWebView *pageWeb = new QWebView;

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(pageWeb);
    pageOnglet->setLayout(layout);

    SoraInfo *soraInfo = new SoraInfo();
    m_sowarInfo.append(soraInfo);

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

SoraInfo *KTab::currentSoraInfo()
{
    return m_sowarInfo.value(this->currentIndex());
}

void KTab::closeTab(int tabIndex)
{
    this->removeTab(tabIndex);
    m_sowarInfo.removeAt(tabIndex);
    emit reloadCurrentSoraInfo();

    // Let's make sure that the last tab well never get closed!
    if(this->count() == 1)
        this->setTabsClosable(false);
}
