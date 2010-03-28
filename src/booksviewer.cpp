#include "booksviewer.h"

BooksViewer::BooksViewer(QMainWindow *parent): QMainWindow(parent->centralWidget())
{
    m_indexDock = new IndexDockWidget();

    this->addDockWidget(Qt::RightDockWidgetArea, m_indexDock);
    this->setLayoutDirection(Qt::RightToLeft);
    this->setAttribute(Qt::WA_DeleteOnClose);

    QWidget *tabWidget = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout;
    m_tab = new KTab(tabWidget);

    layout->addWidget(m_tab);
    layout->setMargin(5);
    tabWidget->setLayout(layout);

    this->setCentralWidget(tabWidget);

    m_quranModel = new QuranTextModel();
    m_quranModel->openQuranDB("books/quran.db");
    QStringListModel *quranIndex = new QStringListModel();
    m_quranModel->getSowarList(quranIndex);
    m_indexDock->setIndex(quranIndex);

    connect(m_tab, SIGNAL(tabCloseRequested(int)), m_tab, SLOT(closeTab(int)));
    connect(m_tab, SIGNAL(currentChanged(int)), this, SLOT(updateSoraDetials()));
    connect(m_tab, SIGNAL(reloadCurrentPageInfo()), this, SLOT(updateSoraDetials()));
    connect(m_indexDock, SIGNAL(ayaNumberChange(int)), this, SLOT(ayaNumberChange(int)));
    connect(m_indexDock, SIGNAL(openSora(int)), this, SLOT(openSora(int)));
    connect(m_indexDock, SIGNAL(openSoraInNewTab(int)), this, SLOT(openSoraInNewTab(int)));

    createMenus(parent);
}

BooksViewer::~BooksViewer()
{
}

void BooksViewer::createMenus(QMainWindow *parent)
{

    // General Actions
    actionNewTab = new QAction(QIcon(":/menu/images/bookmark-new.png"),
                                        trUtf8("تبويب جديد"),
                                        this);

    actionIndexDock = new QAction(QIcon(":/menu/images/edit_fahrass.png"),
                                           trUtf8("نافذة الفهرس"),
                                           this);

    actionSearchDock = new QAction(QIcon(":/menu/images/find.png"),
                                            trUtf8("نافذة البحث"),
                                            this);

    actionIndexDock->setCheckable(true);
    actionSearchDock->setCheckable(true);

    // Navigation actions
    actionNextAYA = new QAction(QIcon(":/menu/images/go-first.png"),
                                trUtf8("الآية التالية"),
                                this);
    actionNextPage = new QAction(QIcon(":/menu/images/go-previous.png"),
                                 trUtf8("الصفحة التالية"),
                                 this);
    actionPrevAYA = new QAction(QIcon(":/menu/images/go-last.png"),
                                trUtf8("الآية السابقة"),
                                this);
    actionPrevPage = new QAction(QIcon(":/menu/images/go-next.png"),
                                 trUtf8("الصفحة السابقة"),
                                 this);

    // Tafressir actions
    openSelectedTafsir =  new QAction(QIcon(":/menu/images/arrow-left.png"),
                                               trUtf8("فتح السورة"),
                                               this);
    comboTafasir = new QComboBox(this);
    comboTafasir->addItem(trUtf8("تفسير ابن كثير"));

    toolBarGeneral = new QToolBar(trUtf8("عام"), this);
    toolBarGeneral->addAction(actionNewTab);
    toolBarGeneral->addSeparator();
    toolBarGeneral->addAction(actionIndexDock);
    toolBarGeneral->addAction(actionSearchDock);

    toolBarNavigation = new QToolBar(trUtf8("التصفح"), this);
    toolBarNavigation->addAction(actionPrevPage);
    toolBarNavigation->addAction(actionPrevAYA);
    toolBarNavigation->addAction(actionNextAYA);
    toolBarNavigation->addAction(actionNextPage);

    toolBarTafesir = new QToolBar(trUtf8("التفسير"), this);
    toolBarTafesir->addWidget(comboTafasir);
    toolBarTafesir->addAction(openSelectedTafsir);

    parent->addToolBar(toolBarGeneral);
    parent->addToolBar(toolBarNavigation);
    parent->addToolBar(toolBarTafesir);

    // Setup connections
    connect(actionNextPage, SIGNAL(triggered()),
            this, SLOT(nextPage()));
    connect(actionPrevPage, SIGNAL(triggered()),
            this, SLOT(previousPage()));
    connect(actionNextAYA, SIGNAL(triggered()),
            this, SLOT(nextAya()));
    connect(actionPrevAYA, SIGNAL(triggered()),
            this, SLOT(previousAYA()));

    connect(actionIndexDock, SIGNAL(toggled(bool)),
            m_indexDock, SLOT(setShown(bool)));
    connect(m_indexDock, SIGNAL(visibilityChanged(bool)),
            this, SLOT(showIndexDock(bool)));

}

void BooksViewer::openSora(int pSoraNumber, int pAyaNumber)
{
    m_quranModel->getPageInfo(pSoraNumber, pAyaNumber, m_tab->currentPageInfo());
    displayPage(m_tab->currentPageInfo());
}

void BooksViewer::displayPage(PageInfo *pPageInfo)
{
    bool emptyPage = m_tab->currentPage()->page()->mainFrame()->toPlainText().isEmpty();

    if(emptyPage or pPageInfo->currentPage() != m_indexDock->currentPageNmber())
        m_tab->currentPage()->page()->mainFrame()->setHtml(m_quranModel->getQuranPage(pPageInfo));

    m_tab->setTabText(m_tab->currentIndex(),
                      trUtf8("سورة %1").arg(pPageInfo->currentSoraName()));
    scrollToAya(pPageInfo->currentSoraNumber(), pPageInfo->currentAya());
    m_indexDock->setSoraDetials(m_tab->currentPageInfo());

    m_indexDock->updatePageAndAyaNum(pPageInfo->currentPage(),
                                     pPageInfo->currentAya());
    updateNavigationButtons();
}

void BooksViewer::scrollToAya(int pSoraNumber, int pAyaNumber)
{

    QWebFrame *frame = m_tab->currentPage()->page()->mainFrame();

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

void BooksViewer::openSoraInNewTab(int pSoraNumber)
{
    m_tab->addNewOnglet();
    openSora(pSoraNumber);
}

void BooksViewer::ayaNumberChange(int pNewAyaNum)
{
    if (m_tab->currentPageInfo()->currentAya() == pNewAyaNum)
        return ;
    int page = m_quranModel->getAyaPageNumber(m_tab->currentPageInfo()->currentSoraNumber(),
                                              pNewAyaNum);
    if(page != m_tab->currentPageInfo()->currentPage()) {
        m_tab->currentPageInfo()->setCurrentPage(page);
    }
    m_tab->currentPageInfo()->setCurrentAya(pNewAyaNum);
    displayPage(m_tab->currentPageInfo());
}

void BooksViewer::updateSoraDetials()
{
    m_indexDock->setSoraDetials(m_tab->currentPageInfo());
}

void BooksViewer::nextAya()
{
    int newAyaNumber = m_tab->currentPageInfo()->currentAya()+1;
    if(newAyaNumber > m_tab->currentPageInfo()->currentSoraAyatCount()) {
        // Goto next SORA
        int nextSora = m_tab->currentPageInfo()->currentSoraNumber()+1;
        // SORA number must be less than 114
        if (nextSora >= 115) {
            // If it's less greater 114 we go to the first SORA in the Quran
            nextSora = 1;
        }
        // Then we select it
        openSora(nextSora);
    } else {
        m_indexDock->updateAyaNumber(newAyaNumber);
    }
}

void BooksViewer::previousAYA()
{
    int newAyaNumber = m_tab->currentPageInfo()->currentAya()-1;
    if(newAyaNumber < 1) {
        // Goto previous SORA
        int prevSora = m_tab->currentPageInfo()->currentSoraNumber()-1;
        // We make sure that our SORA number is greater than 0
        if (prevSora <= 0) {
            // less than 0, go to the last SORA
            prevSora = 114;
        }
        openSora(prevSora);
    } else {
        m_indexDock->updateAyaNumber(newAyaNumber);
    }
}

void BooksViewer::nextPage()
{
    m_quranModel->getPageInfoByPage(m_tab->currentPageInfo()->currentPage()+1,
                                    m_tab->currentPageInfo());

    openSora(m_tab->currentPageInfo()->currentSoraNumber(),
             m_tab->currentPageInfo()->currentAya());
}

void BooksViewer::previousPage()
{
    m_quranModel->getPageInfoByPage(m_tab->currentPageInfo()->currentPage()-1,
                                    m_tab->currentPageInfo());

    openSora(m_tab->currentPageInfo()->currentSoraNumber(),
             m_tab->currentPageInfo()->currentAya());
}

void BooksViewer::updateNavigationButtons()
{
    if(m_tab->currentPageInfo()->currentPage() >= 604)
        actionNextPage->setEnabled(false);
    else
        actionNextPage->setEnabled(true);

    if(m_tab->currentPageInfo()->currentPage() <= 1)
        actionPrevPage->setEnabled(false);
    else
        actionPrevPage->setEnabled(true);
}

void BooksViewer::showIndexDock(bool pShowIndexDock)
{
    Q_UNUSED(pShowIndexDock)
    actionIndexDock->setChecked(m_indexDock->isVisible());
}
