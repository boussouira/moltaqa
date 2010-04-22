#include "booksviewer.h"

BooksViewer::BooksViewer(QMainWindow *parent): QMainWindow(parent->centralWidget())
{
    QWidget *tabWidget = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout;
    m_tab = new KTab(tabWidget);

    layout->addWidget(m_tab);
    layout->setMargin(5);
    tabWidget->setLayout(layout);
    setCentralWidget(tabWidget);

    m_indexWidgetDock = new QDockWidget(trUtf8("الفهرس"), this);
    m_stackedWidget = new QStackedWidget(this);
    m_indexWidgetDock->setWidget(m_stackedWidget);
    m_indexWidgetDock->setMaximumWidth(220);

    addDockWidget(Qt::RightDockWidgetArea, m_indexWidgetDock);

    m_quranSearchDock = new QDockWidget(trUtf8("البحث"), this);
    m_quranSearch = new QuranSearch(this, "books/quran.db");
    m_quranSearchDock->setWidget(m_quranSearch);
    m_quranSearchDock->setVisible(false);
    addDockWidget(Qt::BottomDockWidgetArea, m_quranSearchDock);

    connect(m_tab, SIGNAL(tabCloseRequested(int)), m_tab, SLOT(closeTab(int)));
    connect(m_tab, SIGNAL(currentChanged(int)), m_stackedWidget, SLOT(setCurrentIndex(int)));
    connect(m_tab, SIGNAL(currentChanged(int)), this, SLOT(updateSoraDetials()));
    connect(m_tab, SIGNAL(tabMoved(int,int)), this, SLOT(tabChangePosition(int,int)));
    connect(m_tab, SIGNAL(tabCloseRequested(int)), this, SLOT(tabCloseRequest(int)));

    createMenus(parent);
}

BooksViewer::~BooksViewer()
{
    qDeleteAll(m_databases);
    m_databases.clear();
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
    // New tab
    connect(actionNewTab, SIGNAL(triggered()), this, SLOT(openSoraInNewTab()));

    // Navigation actions
    connect(actionNextPage, SIGNAL(triggered()), this, SLOT(nextPage()));
    connect(actionPrevPage, SIGNAL(triggered()), this, SLOT(previousPage()));
    connect(actionNextAYA, SIGNAL(triggered()), this, SLOT(nextAya()));
    connect(actionPrevAYA, SIGNAL(triggered()), this, SLOT(previousAYA()));

    // Index Dock
    connect(actionIndexDock, SIGNAL(toggled(bool)), m_indexWidgetDock, SLOT(setShown(bool)));
    connect(m_indexWidgetDock, SIGNAL(visibilityChanged(bool)), this, SLOT(showIndexDock(bool)));

    // Search Dock
    connect(actionSearchDock, SIGNAL(toggled(bool)), m_quranSearchDock, SLOT(setShown(bool)));
    connect(m_quranSearchDock, SIGNAL(visibilityChanged(bool)), this, SLOT(showSearchDock(bool)));
    connect(m_quranSearch, SIGNAL(resultSelected(int,int)), this, SLOT(openSora(int,int)));


}

void BooksViewer::openSora(int pSoraNumber, int pAyaNumber)
{
    databaseHandler()->getPageInfo(pSoraNumber, pAyaNumber, pageInfo());
    displayPage(pageInfo());
}

void BooksViewer::displayPage(PageInfo *pPageInfo)
{
    bool emptyPage = m_tab->currentPage()->page()->mainFrame()->toPlainText().isEmpty();

    if(emptyPage || pPageInfo->currentPage() != currentIndexWidget()->currentPageNmber()) {
        QString text = databaseHandler()->getQuranPage(pPageInfo);
        m_tab->currentPage()->page()->mainFrame()->setHtml(text);
    }
    m_tab->setTabText(m_tab->currentIndex(),
                      trUtf8("سورة %1").arg(pPageInfo->currentSoraName()));
    scrollToAya(pPageInfo->currentSoraNumber(), pPageInfo->currentAya());
    currentIndexWidget()->setSoraDetials(pageInfo());

    currentIndexWidget()->updatePageAndAyaNum(pPageInfo->currentPage(),
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
    int tabIndex = m_tab->addNewOnglet();

    IndexWidget *indexWidget = new IndexWidget(this);
    m_stackedWidget->insertWidget(tabIndex, indexWidget);
    m_stackedWidget->setCurrentIndex(tabIndex);

    QuranTextModel *quranModel = new QuranTextModel();
    quranModel->openQuranDB("books/quran.db");
    m_databases.insert(tabIndex, quranModel);

    QStringListModel *quranIndex = new QStringListModel();
    quranModel->getSowarList(quranIndex);
    indexWidget->setIndex(quranIndex);

    openSora(pSoraNumber);

    connect(indexWidget, SIGNAL(ayaNumberChange(int)), this, SLOT(ayaNumberChange(int)));
    connect(indexWidget, SIGNAL(openSora(int)), this, SLOT(openSora(int)));
    connect(indexWidget, SIGNAL(openSoraInNewTab(int)), this, SLOT(openSoraInNewTab(int)));
}

void BooksViewer::ayaNumberChange(int pNewAyaNum)
{
    if (pageInfo()->currentAya() == pNewAyaNum)
        return ;
    int page = databaseHandler()->getAyaPageNumber(pageInfo()->currentSoraNumber(),
                                              pNewAyaNum);
    if(page != pageInfo()->currentPage()) {
        pageInfo()->setCurrentPage(page);
    }
    pageInfo()->setCurrentAya(pNewAyaNum);
    displayPage(pageInfo());
}

void BooksViewer::updateSoraDetials()
{
    if(currentIndexWidget())
        currentIndexWidget()->setSoraDetials(pageInfo());
}

void BooksViewer::nextAya()
{
    int newAyaNumber = pageInfo()->currentAya()+1;
    if(newAyaNumber > pageInfo()->currentSoraAyatCount()) {
        // Goto next SORA
        int nextSora = pageInfo()->currentSoraNumber()+1;
        // SORA number must be less than 114
        if (nextSora >= 115) {
            // If it's greater than 114, we go to the first SORA in the Quran
            nextSora = 1;
        }
        // Then we select it
        openSora(nextSora);
    } else {
        currentIndexWidget()->updateAyaNumber(newAyaNumber);
    }
}

void BooksViewer::previousAYA()
{
    int newAyaNumber = pageInfo()->currentAya()-1;
    if(newAyaNumber < 1) {
        // Goto previous SORA
        int prevSora = pageInfo()->currentSoraNumber()-1;
        // We make sure that our SORA number is greater than 0
        if (prevSora <= 0) {
            // less than 0, go to the last SORA
            prevSora = 114;
        }
        openSora(prevSora);
    } else {
        currentIndexWidget()->updateAyaNumber(newAyaNumber);
    }
}

void BooksViewer::nextPage()
{
    databaseHandler()->getPageInfoByPage(pageInfo()->currentPage()+1,
                                    pageInfo());

    openSora(pageInfo()->currentSoraNumber(),
             pageInfo()->currentAya());
}

void BooksViewer::previousPage()
{
    databaseHandler()->getPageInfoByPage(pageInfo()->currentPage()-1,
                                    pageInfo());

    openSora(pageInfo()->currentSoraNumber(),
             pageInfo()->currentAya());
}

void BooksViewer::updateNavigationButtons()
{
    if(pageInfo()->currentPage() >= 604)
        actionNextPage->setEnabled(false);
    else
        actionNextPage->setEnabled(true);

    if(pageInfo()->currentPage() <= 1)
        actionPrevPage->setEnabled(false);
    else
        actionPrevPage->setEnabled(true);
}

void BooksViewer::showIndexDock(bool pShowIndexDock)
{
    Q_UNUSED(pShowIndexDock)
    actionIndexDock->setChecked(m_indexWidgetDock->isVisible());
}

void BooksViewer::showSearchDock(bool pShowSearchDock)
{
    Q_UNUSED(pShowSearchDock)
    actionSearchDock->setChecked(m_quranSearchDock->isVisible());
}

IndexWidget *BooksViewer::currentIndexWidget()
{
    int currIndex = m_tab->currentIndex();
    // Is there any tabs ?
    if(currIndex >= 0) {
        IndexWidget *indexWidget =  qobject_cast<IndexWidget *>(m_stackedWidget->widget(currIndex));
        return indexWidget;
    } else {      return 0;
    }

}

QuranTextModel *BooksViewer::databaseHandler()
{
    return m_databases.at(m_tab->currentIndex());
}

PageInfo *BooksViewer::pageInfo()
{
    return m_tab->currentPageInfo();
}

void BooksViewer::tabChangePosition(int fromIndex, int toIndex)
{
    m_databases.move(fromIndex, toIndex);
}

void BooksViewer::tabCloseRequest(int tabIndex)
{
    m_databases.removeAt(tabIndex);
}
