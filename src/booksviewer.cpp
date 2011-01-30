#include "booksviewer.h"
#include "tabwidget.h"
#include "indexwidget.h"
#include "bookslistbrowser.h"
#include "simpledbhandler.h"
#include "qurandbhandler.h"
#include "booksindexdb.h"
#include "bookwidget.h"
#include "bookexception.h"

#include <qmainwindow.h>
#include <qmenubar.h>
#include <qmenu.h>
#include <qaction.h>
#include <qtoolbar.h>
#include <qcombobox.h>
#include <qstackedwidget.h>
#include <qboxlayout.h>

BooksViewer::BooksViewer(BooksIndexDB *indexDB, QWidget *parent): QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    m_tab = new TabWidget(this);
    m_indexDB = indexDB;
    layout->addWidget(m_tab);
    layout->setContentsMargins(0,6,0,0);

    connect(m_tab, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
    connect(m_tab, SIGNAL(tabMoved(int,int)), this, SLOT(tabChangePosition(int,int)));
    connect(m_tab, SIGNAL(tabCloseRequested(int)), this, SLOT(tabCloseRequest(int)));
}

BooksViewer::~BooksViewer()
{
}

void BooksViewer::createMenus(QMainWindow *parent)
{

    // General Actions
    actionNewTab = new QAction(QIcon(":/menu/images/bookmark-new.png"),
                                        tr("تبويب جديد"),
                                        this);

    actionIndexDock = new QAction(QIcon(":/menu/images/edit_fahrass.png"),
                                           tr("نافذة الفهرس"),
                                           this);

    actionSearchDock = new QAction(QIcon(":/menu/images/find.png"),
                                            tr("نافذة البحث"),
                                            this);
    actionSearchDock->setEnabled(false);

    // Navigation actions
    actionNextAYA = new QAction(QIcon(":/menu/images/go-down.png"),
                                tr("الآية التالية"),
                                this);
    actionNextPage = new QAction(QIcon(":/menu/images/go-previous.png"),
                                 tr("الصفحة التالية"),
                                 this);
    actionPrevAYA = new QAction(QIcon(":/menu/images/go-up.png"),
                                tr("الآية السابقة"),
                                this);
    actionPrevPage = new QAction(QIcon(":/menu/images/go-next.png"),
                                 tr("الصفحة السابقة"),
                                 this);

    toolBarGeneral = new QToolBar(tr("عام"), this);
    toolBarGeneral->addAction(actionNewTab);
    toolBarGeneral->addSeparator();
    toolBarGeneral->addAction(actionIndexDock);
    toolBarGeneral->addAction(actionSearchDock);

    toolBarNavigation = new QToolBar(tr("التصفح"), this);
    toolBarNavigation->addAction(actionPrevPage);
    toolBarNavigation->addAction(actionNextPage);
    toolBarNavigation->addAction(actionNextAYA);
    toolBarNavigation->addAction(actionPrevAYA);

    parent->addToolBar(toolBarGeneral);
    parent->addToolBar(toolBarNavigation);

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
    BookInfo *bookInfo = m_indexDB->getBookInfo(pBookID);

    if(!bookInfo->exists())
        throw BookException(tr("لم يتم العثور على ملف"), bookInfo->bookPath());

    AbstractDBHandler *bookdb;
    if(bookInfo->isQuran())
        bookdb = new QuranDBHandler();
    else
        bookdb = new SimpleDBHandler();

    bookdb->setBookInfo(bookInfo);

    try {
        bookdb->openBookDB();
    } catch (BookException &e) {
        delete bookdb;
        throw;
    }

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

    connect(bookWidget->indexWidget(), SIGNAL(openPage(int)), SLOT(updateActions()));

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
        updateActions();
    }
}
