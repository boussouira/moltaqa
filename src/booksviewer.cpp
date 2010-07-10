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

    m_infoDB = new BookInfoHandler();

    connect(m_tab, SIGNAL(tabCloseRequested(int)), m_tab, SLOT(closeTab(int)));
    connect(m_tab, SIGNAL(currentChanged(int)), m_stackedWidget, SLOT(setCurrentIndex(int)));
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
    // Navigation actions
    connect(actionNextPage, SIGNAL(triggered()), this, SLOT(nextPage()));
    connect(actionPrevPage, SIGNAL(triggered()), this, SLOT(previousPage()));
    connect(actionNextAYA, SIGNAL(triggered()), this, SLOT(nextUnit()));
    connect(actionPrevAYA, SIGNAL(triggered()), this, SLOT(previousUnit()));

    // Index Dock
    connect(actionIndexDock, SIGNAL(toggled(bool)), m_indexWidgetDock, SLOT(setShown(bool)));
    connect(m_indexWidgetDock, SIGNAL(visibilityChanged(bool)), this, SLOT(showIndexDock(bool)));

    // Search Dock
    connect(actionSearchDock, SIGNAL(toggled(bool)), m_quranSearchDock, SLOT(setShown(bool)));
    connect(m_quranSearchDock, SIGNAL(visibilityChanged(bool)), this, SLOT(showSearchDock(bool)));

}

void BooksViewer::openBook(int pBookID, bool newTab)
{
    BookInfo *bookInfo = m_infoDB->getBookInfo(pBookID);

    AbstractDBHandler *bookdb;
    if(bookInfo->bookType() == BookInfo::QuranBook)
        bookdb = new QuranDBHandler();
    else
        bookdb = new simpleDBHandler();

    bookdb->setBookInfo(bookInfo);
    bookdb->openQuranDB();

    int tabIndex = m_tab->addNewOnglet();

    IndexWidget *indexWidget = new IndexWidget(this);
    m_stackedWidget->insertWidget(tabIndex, indexWidget);
    m_stackedWidget->setCurrentIndex(tabIndex);

    indexWidget->setIndex(bookdb->indexModel());
    m_tab->setTabText(m_tab->currentIndex(), bookdb->bookInfo()->bookName());
    m_databases.append(bookdb);

    m_tab->setPageHtml(bookdb->page());
    connect(indexWidget, SIGNAL(openPage(int)), this, SLOT(openPage(int)));
}

void BooksViewer::nextUnit()
{
    if(databaseHandler()->bookInfo()->bookType() == BookInfo::NormalBook) {
        if(!m_tab->maxDown())
            m_tab->pageDown();
        else
            nextPage();
    }
}

void BooksViewer::previousUnit()
{
    if(databaseHandler()->bookInfo()->bookType() == BookInfo::NormalBook) {
        if(!m_tab->maxUp())
            m_tab->pageUp();
        else
            previousPage();
    }
}

void BooksViewer::nextPage()
{

    m_tab->setPageHtml(databaseHandler()->nextPage());
}

void BooksViewer::previousPage()
{
    m_tab->setPageHtml(databaseHandler()->prevPage());
}

void BooksViewer::updateNavigationButtons()
{

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
    } else {
        return 0;
    }

}

AbstractDBHandler *BooksViewer::databaseHandler()
{
    return m_databases.at(m_tab->currentIndex());
}

void BooksViewer::tabChangePosition(int fromIndex, int toIndex)
{
    m_databases.move(fromIndex, toIndex);

    IndexWidget *indexWidget = qobject_cast<IndexWidget *>(m_stackedWidget->widget(fromIndex));
    m_stackedWidget->removeWidget(indexWidget);
    m_stackedWidget->insertWidget(toIndex, indexWidget);
}

void BooksViewer::tabCloseRequest(int tabIndex)
{
    m_databases.removeAt(tabIndex);

    IndexWidget *indexWidget = qobject_cast<IndexWidget *>(m_stackedWidget->widget(tabIndex));
    m_stackedWidget->removeWidget(indexWidget);
}

void BooksViewer::openPage(int pageID)
{
    if(databaseHandler()->bookInfo()->bookType() == BookInfo::QuranBook) {
        QuranDBHandler *qDB = static_cast<QuranDBHandler*>(databaseHandler());
        m_tab->setPageHtml(qDB->openSora(pageID));
        m_tab->scrollToSora(pageID);
    } else
        m_tab->setPageHtml(databaseHandler()->page(pageID));
}
