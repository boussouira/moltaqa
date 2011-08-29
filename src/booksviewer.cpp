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
    m_actionNewTab = new QAction(QIcon(":/menu/images/bookmark-new.png"),
                                        tr("تبويب جديد"),
                                        this);

    m_actionIndexDock = new QAction(QIcon(":/menu/images/edit_fahrass.png"),
                                           tr("نافذة الفهرس"),
                                           this);

    m_actionSearchDock = new QAction(QIcon(":/menu/images/find.png"),
                                            tr("نافذة البحث"),
                                            this);
    m_actionSearchDock->setEnabled(false);

    // Navigation actions
    m_actionNextAYA = new QAction(QIcon(":/menu/images/go-down.png"),
                                tr("الآية التالية"),
                                this);
    m_actionNextPage = new QAction(QIcon(":/menu/images/go-previous.png"),
                                 tr("الصفحة التالية"),
                                 this);
    m_actionPrevAYA = new QAction(QIcon(":/menu/images/go-up.png"),
                                tr("الآية السابقة"),
                                this);
    m_actionPrevPage = new QAction(QIcon(":/menu/images/go-next.png"),
                                 tr("الصفحة السابقة"),
                                 this);
    m_actionFirstPage = new QAction(QIcon(":/menu/images/go-last.png"),
                                tr("الصفحة الاولى"),
                                this);
    m_actionLastPage = new QAction(QIcon(":/menu/images/go-first.png"),
                                 tr("الصفحة الاخيرة"),
                                 this);
    m_actionGotToPage = new QAction(tr("انتقل الى..."),
                                  this);

    // Tafressir actions
    m_openSelectedTafsir =  new QAction(QIcon(":/menu/images/arrow-left.png"),
                                               tr("فتح تفسير الاية"),
                                               this);
    m_comboTafasir = new QComboBox(this);
    foreach(Pair pair, m_indexDB->getTafassirList()) {
        m_comboTafasir->addItem(pair.second, pair.first);
    }

    // Shorooh action
    m_actionOpenShareeh = new QAction(QIcon(":/menu/images/arrow-left.png"),
                                     tr("فتح الشرح"),
                                     this);
    // Add action to their toolbars
    m_toolBarGeneral = new QToolBar(tr("عام"), this);
    m_toolBarGeneral->addAction(m_actionNewTab);
    m_toolBarGeneral->addSeparator();
    m_toolBarGeneral->addAction(m_actionIndexDock);
    m_toolBarGeneral->addAction(m_actionSearchDock);

    m_toolBarNavigation = new QToolBar(tr("التصفح"), this);
    m_toolBarNavigation->addAction(m_actionPrevPage);
    m_toolBarNavigation->addAction(m_actionNextPage);
    m_toolBarNavigation->addAction(m_actionNextAYA);
    m_toolBarNavigation->addAction(m_actionPrevAYA);

    m_toolBarTafesir = new QToolBar(tr("التفاسير"), this);
    m_toolBarTafesir->addWidget(m_comboTafasir);
    m_toolBarTafesir->addAction(m_openSelectedTafsir);

    m_toolBarShorooh = new QToolBar(tr("الشروح"), this);
    m_toolBarShorooh->addAction(m_actionOpenShareeh);

    QMenu *navMenu = new QMenu(tr("التنقل"), this);
    navMenu->addAction(m_actionFirstPage);
    navMenu->addAction(m_actionPrevPage);
    navMenu->addAction(m_actionNextPage);
    navMenu->addAction(m_actionLastPage);
    navMenu->addSeparator();
    navMenu->addAction(m_actionGotToPage); // TODO: implement this


    // Hide those toolbars
    m_toolBarGeneral->hide();
    m_toolBarNavigation->hide();
    m_toolBarTafesir->hide();
    m_toolBarShorooh->hide();

    parent->addToolBar(m_toolBarGeneral);
    parent->addToolBar(m_toolBarNavigation);
    parent->addToolBar(m_toolBarTafesir);
    parent->addToolBar(m_toolBarShorooh);

    QAction *act = parent->menuBar()->actions().at(1);
    m_navMenu = parent->menuBar()->insertMenu(act, navMenu);
    m_navMenu->setVisible(false);

    // Setup connections
    // Navigation actions
    connect(m_actionNextPage, SIGNAL(triggered()), SLOT(nextPage()));
    connect(m_actionPrevPage, SIGNAL(triggered()), SLOT(previousPage()));
    connect(m_actionNextAYA, SIGNAL(triggered()), SLOT(nextUnit()));
    connect(m_actionPrevAYA, SIGNAL(triggered()), SLOT(previousUnit()));
    connect(m_actionFirstPage, SIGNAL(triggered()), SLOT(firstPage()));
    connect(m_actionLastPage, SIGNAL(triggered()), SLOT(lastPage()));
    connect(m_actionGotToPage, SIGNAL(triggered()), SLOT(goToPage()));

    // Index widget
    connect(m_actionIndexDock, SIGNAL(triggered()), SLOT(showIndexWidget()));
    connect(m_actionNewTab, SIGNAL(triggered()), MainWindow::mainWindow(), SLOT(showBooksList()));

    // Tafessir actions
    connect(m_openSelectedTafsir, SIGNAL(triggered()), SLOT(openTafessir()));

    // Shareeh action
    connect(m_actionOpenShareeh, SIGNAL(triggered()), SLOT(openShareeh()));
}

void BooksViewer::removeToolBar()
{
    m_toolBarGeneral->hide();
    m_toolBarNavigation->hide();
    m_toolBarTafesir->hide();
    m_toolBarShorooh->hide();
    m_navMenu->setVisible(false);
}

void BooksViewer::showToolBar()
{
    m_toolBarGeneral->show();
    m_toolBarNavigation->show();
    m_navMenu->setVisible(true);
}

void BooksViewer::openBook(int bookID, int pageID, bool newTab)
{
    BookInfo *bookInfo = m_indexDB->getBookInfo(bookID);

    if(!bookInfo || !bookInfo->exists())
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
    } catch (BookException &) {
        delete bookdb;
        throw;
    }

    BookWidget *bookWidget = new BookWidget(bookdb, this);
    int tabIndex;
    if(newTab) {
        m_bookWidgets.append(bookWidget);
        tabIndex = m_tab->addTab(bookWidget, bookdb->bookInfo()->bookDisplayName);
    } else {
        tabIndex = m_tab->currentIndex();
        m_tab->removeTab(tabIndex);
        m_tab->insertTab(tabIndex, bookWidget, bookdb->bookInfo()->bookDisplayName);
        m_bookWidgets.replace(tabIndex, bookWidget);
    }

    m_tab->setCurrentIndex(tabIndex);
    if(pageID == -1)
        bookWidget->firstPage();
    else
        bookWidget->openID(pageID);

    connect(bookWidget->indexWidget(), SIGNAL(openPage(int)), SLOT(updateActions()));

    updateActions();
    activateWindow();
}

void BooksViewer::openTafessir()
{
    int tafessirID = m_comboTafasir->itemData(m_comboTafasir->currentIndex()).toInt();
    qDebug("Open tafessir: %d", tafessirID);
    BookInfo *bookInfo = m_indexDB->getBookInfo(tafessirID);
    if(!bookInfo || !bookInfo->isTafessir() || !currentBookWidget()->dbHandler()->bookInfo()->isQuran())
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
    int tabIndex = m_tab->addTab(bookWidget, bookdb->bookInfo()->bookDisplayName);
    m_tab->setCurrentIndex(tabIndex);

    bookWidget->openSora(sora, aya);

    updateActions();
}

void BooksViewer::openShareeh()
{
    BookInfo *info = currentBookWidget()->dbHandler()->bookInfo();

    if(info->shorooh.isEmpty())
        return;

    QMenu menu(this);

    foreach(BookShorooh *shareeh, info->shorooh) {
        QAction *act = new QAction(shareeh->bookName, &menu);
        act->setData(QString("%1;%2").arg(shareeh->bookID).arg(shareeh->pageID));

        menu.addAction(act);
    }

    QAction *ret = menu.exec(QCursor::pos());

    if(ret) {
        QStringList retList = ret->data().toString().split(";", QString::SkipEmptyParts);
        int bookID = retList.first().toInt();
        int pageID = retList.last().toInt();

        openBook(bookID, pageID);
    }
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
    dialog.setBookInfo(currentBookWidget()->dbHandler()->bookInfo());

    if(dialog.exec() == QDialog::Accepted) {
        if(dialog.currentPage() == 0) // Open selected page/part
            currentBookWidget()->openPage(dialog.selectedPage(), dialog.selectedPart());
        else if(dialog.currentPage() == 1) // Open selected sora/page
            currentBookWidget()->openSora(dialog.selectedSora(), dialog.selectedAya());
        else if(dialog.currentPage() == 2) // Open selected haddit
            currentBookWidget()->openHaddit(dialog.selectedHaddit());
        else
            qDebug("What to do?");
    }
}

void BooksViewer::updateActions()
{
    bool hasNext = currentBookWidget()->dbHandler()->hasNext();
    bool hasPrev = currentBookWidget()->dbHandler()->hasPrev();

    m_actionNextPage->setEnabled(hasNext);
    m_actionLastPage->setEnabled(hasNext);
    m_actionPrevPage->setEnabled(hasPrev);
    m_actionFirstPage->setEnabled(hasPrev);
    m_toolBarShorooh->setEnabled(currentBookWidget()->dbHandler()->bookInfo()->isNormal() &&
                                 !currentBookWidget()->dbHandler()->bookInfo()->shorooh.isEmpty());
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
        bool showShorooh = currentBookWidget()->dbHandler()->bookInfo()->isNormal();

        m_toolBarTafesir->setVisible(showTafsssir);
        m_toolBarTafesir->setEnabled(showTafsssir);

        m_toolBarShorooh->setVisible(showShorooh);
        m_toolBarShorooh->setEnabled(showShorooh && !currentBookWidget()->dbHandler()->bookInfo()->shorooh.isEmpty());

        if(currentBookWidget()->dbHandler()->bookInfo()->isQuran()) {
            m_actionNextAYA->setText(tr("الآية التالية"));
            m_actionPrevAYA->setText(tr("الآية السابقة"));
        } else {
            m_actionNextAYA->setText(tr("انزال الصفحة")); // TODO: fix this tooltip?
            m_actionPrevAYA->setText(tr("رفع الصفحة"));
        }
    }
}
