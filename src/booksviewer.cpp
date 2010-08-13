#include "booksviewer.h"
#include "tabwidget.h"
#include "indexwidget.h"
#include "bookslistbrowser.h"
#include "simpledbhandler.h"
#include "qurandbhandler.h"
#include "bookinfohandler.h"
#include "bookwidget.h"

#include <qmainwindow.h>
#include <qmenubar.h>
#include <qmenu.h>
#include <qaction.h>
#include <qtoolbar.h>
#include <qcombobox.h>
#include <qstackedwidget.h>
#include <qboxlayout.h>

BooksViewer::BooksViewer(QWidget *parent): QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    m_tab = new TabWidget(this);
    layout->addWidget(m_tab);
    layout->setContentsMargins(0,6,0,0);

    m_infoDB = new BookInfoHandler();

    connect(m_tab, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
    connect(m_tab, SIGNAL(tabMoved(int,int)), this, SLOT(tabChangePosition(int,int)));
    connect(m_tab, SIGNAL(tabCloseRequested(int)), this, SLOT(tabCloseRequest(int)));
}

BooksViewer::~BooksViewer()
{
    delete m_infoDB;
    QSqlDatabase::removeDatabase("BooksInfoDB");
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
    actionSearchDock->setEnabled(false);

    // Navigation actions
    actionNextAYA = new QAction(QIcon(":/menu/images/go-down.png"),
                                trUtf8("الآية التالية"),
                                this);
    actionNextPage = new QAction(QIcon(":/menu/images/go-previous.png"),
                                 trUtf8("الصفحة التالية"),
                                 this);
    actionPrevAYA = new QAction(QIcon(":/menu/images/go-up.png"),
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
    toolBarNavigation->addAction(actionNextPage);
    toolBarNavigation->addAction(actionNextAYA);
    toolBarNavigation->addAction(actionPrevAYA);

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

    // Index widget
    connect(actionIndexDock, SIGNAL(triggered()), this, SLOT(showIndexWidget()));
}

void BooksViewer::openBook(int pBookID, bool newTab)
{
    BookInfo *bookInfo = m_infoDB->getBookInfo(pBookID);

    AbstractDBHandler *bookdb;
    if(bookInfo->isQuran())
        bookdb = new QuranDBHandler();
    else
        bookdb = new SimpleDBHandler();

    bookdb->setBookInfo(bookInfo);
    bookdb->openBookDB();
    BookWidget *bookWidget = new BookWidget(bookdb, this);
    int tabIndex;
    if(newTab) {
        m_bookWidgets.append(bookWidget);
        tabIndex = m_tab->addTab(bookWidget, bookdb->bookInfo()->bookName());
    } else {
        tabIndex = m_tab->currentIndex();
        m_tab->removeTab(tabIndex);
        m_tab->insertTab(tabIndex, bookWidget, bookdb->bookInfo()->bookName());
        m_bookWidgets.replace(tabIndex, bookWidget);
    }

    m_tab->setCurrentIndex(tabIndex);
    bookWidget->firstPage();
    updateActions();
}

void BooksViewer::nextUnit()
{
    currentBookWidget()->nextUnit();
    updateActions();
}

void BooksViewer::previousUnit()
{
    currentBookWidget()->prevUnit();
    updateActions();
}

void BooksViewer::nextPage()
{
    currentBookWidget()->nextPage();
    updateActions();
}

void BooksViewer::previousPage()
{
    currentBookWidget()->prevPage();
    updateActions();
}

void BooksViewer::updateActions()
{
    actionNextPage->setEnabled(currentBookWidget()->dbHandler()->hasNext());
    actionPrevPage->setEnabled(currentBookWidget()->dbHandler()->hasPrev());
}

void BooksViewer::showIndexWidget()
{
    currentBookWidget()->hideIndexWidget();
}

BookWidget *BooksViewer::currentBookWidget()
{
    return currentBookWidget(m_tab->currentIndex());
}

BookWidget *BooksViewer::currentBookWidget(int index)
{
    return (m_bookWidgets.count() > 0) ? m_bookWidgets.at(index) : NULL;
}

void BooksViewer::tabChangePosition(int fromIndex, int toIndex)
{
    m_bookWidgets.move(fromIndex, toIndex);
}

void BooksViewer::tabCloseRequest(int tabIndex)
{
    m_bookWidgets.removeAt(tabIndex);
    m_tab->removeTab(tabIndex);
}

void BooksViewer::tabChanged(int newIndex)
{
    if(newIndex != -1) {
        bool showTafsssir = currentBookWidget()->dbHandler()->bookInfo()->isQuran();
        toolBarTafesir->setVisible(showTafsssir);
        toolBarTafesir->setEnabled(showTafsssir);
        updateActions();
    }
}
