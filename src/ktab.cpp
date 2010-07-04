#include "ktab.h"

KTab::KTab(QWidget *parent) : QTabWidget(parent), m_tab(new QTabBar(this))
{
    setTabsClosable(false);
    setTabBar(m_tab);
    setMovable(true);

    connect(m_tab, SIGNAL(tabMoved(int, int)), this, SLOT(tabIsMoved(int,int)));

}

QWidget *KTab::newOnglet(BookInfo::Type pBookType)
{
    Q_UNUSED(pBookType)
    QWidget *pageOnglet = new QWidget;
    QWebView *pageWeb = new QWebView;

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(pageWeb);
    pageOnglet->setLayout(layout);

    return pageOnglet;
}

int KTab::addNewOnglet(BookInfo::Type pBookType)
{
    int newTabIndex = addTab(newOnglet(pBookType), trUtf8("القرآن الكريم"));
    setCurrentIndex(newTabIndex);

    if(count() > 1)
        setTabsClosable(true);
    return newTabIndex;
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

void KTab::setPageHtml(const QString &text)
{
    currentPage()->page()->mainFrame()->setHtml(text);
}

void KTab::scrollToAya(int pSoraNumber, int pAyaNumber)
{

    QWebFrame *frame = currentPage()->page()->mainFrame();

    // First we unhighlight the highlighted AYA
    frame->findFirstElement("span.highlighted").removeClass("highlighted");

    // Since each AYA has it own uniq id, we can highlight
    // any AYA in the current page by adding the class "highlighted"
    frame->findFirstElement(QString("span#s%1a%2")
                            .arg(pSoraNumber).arg(pAyaNumber)).addClass("highlighted");

    // Get the postion of the selected AYA
    QRect highElement = frame->findFirstElement("span.highlighted").geometry();
    // Frame heihgt
    int frameHeihgt = frame->geometry().height() / 2;
    // The height that should be added to center the selected aya
    int addHeight = highElement.height() / 2 ;
    // it must be less than frameHeight
    while (frameHeihgt < addHeight )
        addHeight = addHeight / 2;
    // The aya position equal ((ayaHeight - frameHeight) + addHeight)
    unsigned int ayaPosition = (highElement.y() - frameHeihgt) + addHeight;

    // Animation the scrolling to the selected AYA
    QPropertyAnimation *animation = new QPropertyAnimation(frame, "scrollPosition");
    animation->setDuration(1000);
    animation->setStartValue(frame->scrollPosition());
    animation->setEndValue(QPoint(0, ayaPosition));

    animation->start();
}
