#include "booksviewer.h"
#include "tabwidget.h"
#include "indexwidget.h"
#include "bookslistbrowser.h"
#include "simpledbhandler.h"
#include "qurandbhandler.h"
#include "tafessirdbhandler.h"
#include "indexdb.h"
#include "bookwidget.h"
#include "bookexception.h"
#include "openpagedialog.h"
#include "mainwindow.h"

#include <qmainwindow.h>
#include <qmenubar.h>
#include <qmenu.h>
#include <qaction.h>
#include <qtoolbar.h>
#include <qcombobox.h>
#include <qstackedwidget.h>
#include <qboxlayout.h>
#include <qdebug.h>
#include <qmessagebox.h>

typedef QPair<int, QString> Pair;

BooksViewer::BooksViewer(IndexDB *indexDB, QMainWindow *parent): QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    m_tab = new TabWidget(this);
    m_indexDB = indexDB;
    layout->addWidget(m_tab);
    layout->setContentsMargins(0,6,0,0);

    createMenus(parent);

    connect(m_tab, SIGNAL(currentChanged(int)), SLOT(tabChanged(int)));
    connect(m_tab, SIGNAL(tabMoved(int,int)), SLOT(tabChangePosition(int,int)));
    connect(m_tab, SIGNAL(tabCloseRequested(int)), SLOT(tabCloseRequest(int)));
    connect(m_tab, SIGNAL(lastTabClosed()), SIGNAL(lastTabClosed()));
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
    actionFirstPage = new QAction(QIcon(":/menu/images/go-last.png"),
                                tr("الصفحة الاولى"),
                                this);
    actionLastPage = new QAction(QIcon(":/menu/images/go-first.png"),
                                 tr("الصفحة الاخيرة"),
                                 this);
    actionGotToPage = new QAction(tr("انتقل الى الصفحة..."),
                                  this);

    // Tafressir actions
    openSelectedTafsir =  new QAction(QIcon(":/menu/images/arrow-left.png"),
                                               trUtf8("فتح السورة"),
                                               this);
    comboTafasir = new QComboBox(this);
    foreach(Pair pair, m_indexDB->getTafassirList()) {
        comboTafasir->addItem(pair.second, pair.first);
    }

    // Add action to their toolbars
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

    toolBarTafesir = new QToolBar(trUtf8("التفسير"), this);
    toolBarTafesir->addWidget(comboTafasir);
    toolBarTafesir->addAction(openSelectedTafsir);

    QMenu *navMenu = new QMenu(tr("التنقل"), this);
    navMenu->addAction(actionFirstPage);
    navMenu->addAction(actionPrevPage);
    navMenu->addAction(actionNextPage);
    navMenu->addAction(actionLastPage);
    navMenu->addSeparator();
    navMenu->addAction(actionGotToPage); // TODO: implement this


    // Hide those toolbars
    toolBarGeneral->hide();
    toolBarNavigation->hide();
    toolBarTafesir->hide();

    parent->addToolBar(toolBarGeneral);
    parent->addToolBar(toolBarNavigation);
    parent->addToolBar(toolBarTafesir);

    QAction *act = parent->menuBar()->actions().at(1);
    m_navMenu = parent->menuBar()->insertMenu(act, navMenu);
    m_navMenu->setVisible(false);

    // Setup connections
    // Navigation actions
    connect(actionNextPage, SIGNAL(triggered()), SLOT(nextPage()));
    connect(actionPrevPage, SIGNAL(triggered()), SLOT(previousPage()));
    connect(actionNextAYA, SIGNAL(triggered()), SLOT(nextUnit()));
    connect(actionPrevAYA, SIGNAL(triggered()), SLOT(previousUnit()));
    connect(actionFirstPage, SIGNAL(triggered()), SLOT(firstPage()));
    connect(actionLastPage, SIGNAL(triggered()), SLOT(lastPage()));
    connect(actionGotToPage, SIGNAL(triggered()), SLOT(goToPage()));

    // Index widget
    connect(actionIndexDock, SIGNAL(triggered()), SLOT(showIndexWidget()));
    connect(actionNewTab, SIGNAL(triggered()), MainWindow::mainWindow(), SLOT(showBooksList()));

    // Tafessir actions
    connect(openSelectedTafsir, SIGNAL(triggered()), SLOT(openTafessir()));
}

void BooksViewer::removeToolBar()
{
    toolBarGeneral->hide();
    toolBarNavigation->hide();
    toolBarTafesir->hide();
    m_navMenu->setVisible(false);
}

void BooksViewer::showToolBar()
{
    toolBarGeneral->show();
    toolBarNavigation->show();
    m_navMenu->setVisible(true);
}

void BooksViewer::openBook(int pBookID, bool newTab)
{
    BookInfo *bookInfo = m_indexDB->getBookInfo(pBookID);

    if(!bookInfo->exists())
        throw BookException(tr("لم يتم العثور على ملف"), bookInfo->bookPath);

    AbstractDBHandler *bookdb;
    if(bookInfo->isQuran())
        bookdb = new QuranDBHandler();
    else if(bookInfo->isNormal())
        bookdb = new SimpleDBHandler();
    else if(bookInfo->isTafessir())
        bookdb = new TafessirDBHandler();
    else
        throw BookException(tr("لم يتم التعرف على نوع الكتاب"), QString("Book Type: %1").arg(bookInfo->bookPath));

    bookdb->setConnctionInfo(m_indexDB->connectionInfo());
    bookdb->setBookInfo(bookInfo);
    bookdb->setIndexDB(m_indexDB);

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
        tabIndex = m_tab->addTab(bookWidget, bookdb->bookInfo()->bookName);
    } else {
        tabIndex = m_tab->currentIndex();
        m_tab->removeTab(tabIndex);
        m_tab->insertTab(tabIndex, bookWidget, bookdb->bookInfo()->bookName);
        m_bookWidgets.replace(tabIndex, bookWidget);
    }

    m_tab->setCurrentIndex(tabIndex);
    bookWidget->firstPage();

    connect(bookWidget->indexWidget(), SIGNAL(openPage(int)), SLOT(updateActions()));

    updateActions();
    activateWindow();
}

void BooksViewer::openTafessir()
{
    int tafessirID = comboTafasir->itemData(comboTafasir->currentIndex()).toInt();
    qDebug("Open tafessir: %d", tafessirID);
    BookInfo *bookInfo = m_indexDB->getBookInfo(tafessirID);
    if(!bookInfo->isTafessir() || !currentBookWidget()->dbHandler()->bookInfo()->isQuran())
        return;

    TafessirDBHandler *bookdb = new TafessirDBHandler();
    bookdb->setConnctionInfo(m_indexDB->connectionInfo());
    bookdb->setBookInfo(bookInfo);
    bookdb->setIndexDB(m_indexDB);

    try {
        bookdb->openBookDB();
    } catch (BookException &e) {
        delete bookdb;
        QMessageBox::warning(this,
                             tr("فتح التفسير"),
                             e.what());
    }

    int sora = currentBookWidget()->dbHandler()->bookInfo()->currentSoraNumber;
    int aya = currentBookWidget()->dbHandler()->bookInfo()->currentAyaNumber;

    BookWidget *bookWidget = new BookWidget(bookdb, this);

    m_bookWidgets.append(bookWidget);
    int tabIndex = m_tab->addTab(bookWidget, bookdb->bookInfo()->bookName);
    m_tab->setCurrentIndex(tabIndex);

    bookdb->openSora(sora, aya);

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

void BooksViewer::firstPage()
{
    currentBookWidget()->firstPage();
    updateActions();
}

void BooksViewer::lastPage()
{
    currentBookWidget()->lastPage();
    updateActions();
}

void BooksViewer::goToPage()
{
    OpenPageDialog dialog(this);
    dialog.setPage(currentBookWidget()->dbHandler()->bookInfo()->currentPageNumber);
    dialog.setPart(currentBookWidget()->dbHandler()->bookInfo()->currentPartNumber);

    if(dialog.exec() == QDialog::Accepted) {
        currentBookWidget()->openPage(dialog.selectedPage(), dialog.selectedPart());
    }
}

void BooksViewer::updateActions()
{
    bool hasNext = currentBookWidget()->dbHandler()->hasNext();
    bool hasPrev = currentBookWidget()->dbHandler()->hasPrev();

    actionNextPage->setEnabled(hasNext);
    actionLastPage->setEnabled(hasNext);
    actionPrevPage->setEnabled(hasPrev);
    actionFirstPage->setEnabled(hasPrev);
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
    currentBookWidget()->saveSettings();
    m_bookWidgets.removeAt(tabIndex);
    m_tab->removeTab(tabIndex);
}

void BooksViewer::tabChanged(int newIndex)
{
    if(newIndex != -1) {
        updateActions();

        bool showTafsssir = currentBookWidget()->dbHandler()->bookInfo()->isQuran();
        toolBarTafesir->setVisible(showTafsssir);
        toolBarTafesir->setEnabled(showTafsssir);

        if(currentBookWidget()->dbHandler()->bookInfo()->isQuran()) {
            actionNextAYA->setText(tr("الآية التالية"));
            actionPrevAYA->setText(tr("الآية السابقة"));
        } else {
            actionNextAYA->setText(tr("انزال الصفحة")); // TODO: fix this tooltip?
            actionPrevAYA->setText(tr("رفع الصفحة"));
        }
    }
}
