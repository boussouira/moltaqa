#include "booksviewer.h"
#include "tabwidget.h"
#include "indexwidget.h"
#include "bookslistbrowser.h"
#include "richsimplebookreader.h"
#include "richquranreader.h"
#include "richtafessirreader.h"
#include "librarymanager.h"
#include "bookwidget.h"
#include "bookexception.h"
#include "openpagedialog.h"
#include "modelenums.h"
#include "mainwindow.h"
#include "bookwidgetmanager.h"
#include "utils.h"
#include "bookeditorview.h"
#include "taffesirlistmanager.h"
#include "librarybookmanager.h"

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
#include <qkeysequence.h>
#include <QCompleter>

static BooksViewer *m_instance = 0;

BooksViewer::BooksViewer(LibraryManager *libraryManager, QWidget *parent): AbstarctView(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    m_viewManager = new BookWidgetManager(this);

    m_instance = this;

    m_libraryManager = libraryManager;
    m_bookManager = m_libraryManager->bookManager();
    m_taffesirManager = m_libraryManager->taffesirListManager();

    layout->addWidget(m_viewManager);
    layout->setContentsMargins(0,6,0,0);

    createMenus();
    loadTafessirList();

    connect(m_viewManager, SIGNAL(currentTabChanged(int)), SLOT(updateActions()));
    connect(m_viewManager, SIGNAL(currentTabChanged(int)), SLOT(tabChanged(int)));
    connect(m_viewManager, SIGNAL(lastTabClosed()), SIGNAL(lastTabClosed()));
    connect(m_viewManager, SIGNAL(lastTabClosed()), SIGNAL(hideMe()));
}

BooksViewer::~BooksViewer()
{
    delete m_viewManager;

    m_instance = 0;
}

BooksViewer *BooksViewer::instance()
{
    return m_instance;
}

QString BooksViewer::title()
{
    return tr("تصفح الكتب");
}

void BooksViewer::createMenus()
{

    // Edit book action
    m_actionEditBook = new QAction(QIcon::fromTheme("document-edit"),
                                 tr("تحرير الكتاب"), this);
    // General Actions
    m_actionNewTab = new QAction(QIcon::fromTheme("tab-new", QIcon(":/images/bookmark-new.png")),
                                        tr("تبويب جديد"),
                                        this);

    m_actionIndexDock = new QAction(QIcon(":/images/edit_fahrass.png"),
                                           tr("نافذة الفهرس"),
                                           this);

    m_actionSearchInBook = new QAction(QIcon::fromTheme("edit-find", QIcon(":/images/find.png")),
                                            tr("بحث متقدم في هذا الكتاب"),
                                            this);

    // Navigation actions
    m_actionNextAYA = new QAction(QIcon::fromTheme("go-down", QIcon(":/images/go-down.png")),
                                tr("الآية التالية"),
                                this);
    m_actionNextPage = new QAction(QIcon::fromTheme("go-previous", QIcon(":/images/go-previous.png")),
                                 tr("الصفحة التالية"),
                                 this);
    m_actionPrevAYA = new QAction(QIcon::fromTheme("go-up", QIcon(":/images/go-up.png")),
                                tr("الآية السابقة"),
                                this);
    m_actionPrevPage = new QAction(QIcon::fromTheme("go-next", QIcon(":/images/go-next.png")),
                                 tr("الصفحة السابقة"),
                                 this);
    m_actionFirstPage = new QAction(QIcon::fromTheme("go-last", QIcon(":/images/go-last.png")),
                                tr("الصفحة الاولى"),
                                this);
    m_actionLastPage = new QAction(QIcon::fromTheme("go-first", QIcon(":/images/go-first.png")),
                                 tr("الصفحة الاخيرة"),
                                 this);
    m_actionGotToPage = new QAction(QIcon::fromTheme("go-jump"), tr("انتقل الى..."),
                                  this);

    m_actionNextAYA->setShortcut(QKeySequence("J"));
    m_actionPrevAYA->setShortcut(QKeySequence("K"));
    m_actionNextPage->setShortcut(QKeySequence("N"));
    m_actionPrevPage->setShortcut(QKeySequence("P"));
    m_actionGotToPage->setShortcut(QKeySequence("G"));

    // Tafressir actions
    m_openSelectedTafsir =  new QAction(QIcon(":/images/arrow-left.png"),
                                               tr("فتح تفسير الاية"),
                                               this);
    m_comboTafasir = new QComboBox(this);

    // Add action to their toolbars
    m_toolBarGeneral = new QToolBar(tr("عام"), this);
    m_toolBarGeneral->addAction(m_actionNewTab);
    m_toolBarGeneral->addSeparator();
    m_toolBarGeneral->addAction(m_actionIndexDock);
    m_toolBarGeneral->addAction(m_actionSearchInBook);

    m_toolBarNavigation = new QToolBar(tr("التصفح"), this);
    m_toolBarNavigation->addAction(m_actionPrevPage);
    m_toolBarNavigation->addAction(m_actionNextPage);
    m_toolBarNavigation->addAction(m_actionNextAYA);
    m_toolBarNavigation->addAction(m_actionPrevAYA);

    m_toolBarTafesir = new QToolBar(tr("التفاسير"), this);
    m_toolBarTafesir->addWidget(m_comboTafasir);
    m_toolBarTafesir->addAction(m_openSelectedTafsir);

    m_navActions << m_actionEditBook;
    m_navActions << actionSeparator(this);
    m_navActions << m_actionFirstPage;
    m_navActions << m_actionPrevPage;
    m_navActions << m_actionNextPage;
    m_navActions << m_actionLastPage;
    m_navActions << m_actionGotToPage;

    m_toolBars << m_toolBarGeneral;
    m_toolBars << m_toolBarNavigation;
    m_toolBars << m_toolBarTafesir;

    connect(m_viewManager, SIGNAL(pageChanged()), SLOT(updateActions()));
    connect(m_actionEditBook, SIGNAL(triggered()), SLOT(editCurrentBook()));

    // Navigation actions
    connect(m_actionNextPage, SIGNAL(triggered()), m_viewManager, SLOT(nextPage()));
    connect(m_actionPrevPage, SIGNAL(triggered()), m_viewManager, SLOT(previousPage()));
    connect(m_actionNextAYA, SIGNAL(triggered()), m_viewManager, SLOT(nextAya()));
    connect(m_actionPrevAYA, SIGNAL(triggered()), m_viewManager, SLOT(previousAya()));
    connect(m_actionFirstPage, SIGNAL(triggered()), m_viewManager, SLOT(firstPage()));
    connect(m_actionLastPage, SIGNAL(triggered()), m_viewManager, SLOT(lastPage()));
    connect(m_actionGotToPage, SIGNAL(triggered()), m_viewManager, SLOT(goToPage()));

    // Generale actions
    connect(m_actionIndexDock, SIGNAL(triggered()), SLOT(showIndexWidget()));
    connect(m_actionNewTab, SIGNAL(triggered()), MW, SLOT(showBooksList()));
    connect(m_actionSearchInBook, SIGNAL(triggered()), this, SLOT(searchInBook()));

    // Tafessir actions
    connect(m_openSelectedTafsir, SIGNAL(triggered()), SLOT(openTafessir()));
    connect(m_taffesirManager, SIGNAL(ModelsReady()), SLOT(loadTafessirList()));
}

void BooksViewer::updateToolBars()
{
    LibraryBookPtr book = m_viewManager->activeBook();

    if(book) {
        bool showTafsssir = book->isQuran() && m_comboTafasir->count();

        m_toolBarTafesir->setEnabled(showTafsssir);
        m_toolBarTafesir->setVisible(showTafsssir);

        m_toolBarGeneral->show();
        m_toolBarNavigation->show();
    }
}

QString BooksViewer::viewLink()
{
    RichBookReader *bookReader = m_viewManager->activeBookReader();
    ML_ASSERT_RET(bookReader, QString());

    QString link = QString("moltaqa://open/");
    if(bookReader->bookInfo()->isQuran())
        link.append(QString("quran?sora=%1&aya=%2").arg(bookReader->page()->sora).arg(bookReader->page()->aya));
    else
        link.append(QString("book?id=%1&page=%2").arg(bookReader->bookInfo()->id).arg(bookReader->page()->pageID));

    return link;
}

int BooksViewer::currentBookID()
{
    LibraryBookPtr book = m_viewManager->activeBook();

    return book ? book->id : 0;
}

LibraryBookPtr BooksViewer::currentBook()
{
    return m_viewManager->activeBook();
}

BookPage *BooksViewer::currentPage()
{
    RichBookReader *bookdb = m_viewManager->activeBookReader();

   return bookdb ? bookdb->page() : 0;
}

BookWidget *BooksViewer::openBook(int bookID, int pageID, CLuceneQuery *query)
{
    LibraryBookPtr bookInfo;
    RichBookReader *bookReader = 0;
    BookWidget *bookWidget = 0;

    try {
        bookInfo = m_bookManager->getLibraryBook(bookID);

        if(!bookInfo)
            throw BookException(tr("لم يتم العثور على الكتاب المطلوب"), tr("معرف الكتاب: %1").arg(bookID));

        if(!bookInfo->exists())
            throw BookException(tr("لم يتم العثور على ملف"), bookInfo->path);

        if(bookInfo->isQuran())
            bookReader = new RichQuranReader();
        else if(bookInfo->isNormal())
            bookReader = new RichSimpleBookReader();
        else if(bookInfo->isTafessir())
            bookReader = new RichTafessirReader();
        else
            throw BookException(tr("لم يتم التعرف على نوع الكتاب"), QString("Book Type: %1").arg(bookInfo->path));

        bookReader->setBookInfo(bookInfo);

        bookReader->openBook();

        if(query && pageID != -1)
            bookReader->highlightPage(pageID, query);

        bookWidget = new BookWidget(bookReader, this);
        m_viewManager->addBook(bookWidget);

        if(pageID == -1)
            bookWidget->openPage(m_bookManager->bookLastPageID(bookID));
        else
            bookWidget->openPage(pageID);

        connect(bookWidget->indexWidget(), SIGNAL(openPage(int)), SLOT(updateActions()));

        updateActions();
        activateWindow();

        emit showMe();

    } catch (BookException &e) {
        QMessageBox::critical(this,
                              tr("فتح كتاب"),
                              e.what());

        ML_DELETE_CHECK(bookReader);
        ML_DELETE_CHECK(bookWidget);
    }

    return bookWidget;
}

void BooksViewer::openTafessir()
{
    BookWidget *bookWidget = 0;
    RichTafessirReader *bookdb = 0;

    try {
        int tafessirID = m_comboTafasir->itemData(m_comboTafasir->currentIndex(), ItemRole::idRole).toInt();

        LibraryBookPtr bookInfo = m_bookManager->getLibraryBook(tafessirID);
        ML_ASSERT(bookInfo && bookInfo->isTafessir() && m_viewManager->activeBook()->isQuran());

        bookdb = new RichTafessirReader();
        bookdb->setBookInfo(bookInfo);

        bookdb->openBook();

        int sora = m_viewManager->activeBookReader()->page()->sora;
        int aya = m_viewManager->activeBookReader()->page()->aya;

        bookWidget = new BookWidget(bookdb, this);
        m_viewManager->addBook(bookWidget);

        bookWidget->openSora(sora, aya);

        updateActions();
    } catch (BookException &e) {
        ML_DELETE_CHECK(bookdb);
        ML_DELETE_CHECK(bookWidget);

        QMessageBox::warning(this,
                             tr("فتح التفسير"),
                             e.what());
    }
}

void BooksViewer::updateActions()
{
    if(m_viewManager->activeBookWidget()) {
        bool hasNext = m_viewManager->activeBookReader()->hasNext();
        bool hasPrev = m_viewManager->activeBookReader()->hasPrev();

        m_actionNextPage->setEnabled(hasNext);
        m_actionLastPage->setEnabled(hasNext);
        m_actionPrevPage->setEnabled(hasPrev);
        m_actionFirstPage->setEnabled(hasPrev);
    }
}

void BooksViewer::showIndexWidget()
{
    BookWidget *book = m_viewManager->activeBookWidget();

    if(book)
        book->hideIndexWidget();
}

void BooksViewer::searchInBook()
{
    LibraryBookPtr book = m_viewManager->activeBook();
    ML_ASSERT(book);

    MW->searchView()->newTab(SearchWidget::BookSearch, book->id);
    MW->showSearchView();
}

void BooksViewer::tabChanged(int newIndex)
{
    if(newIndex != -1) {
        updateActions();
        updateToolBars();

        if(m_viewManager->activeBook()->isQuran()) {
            m_actionNextAYA->setText(tr("الآية التالية"));
            m_actionPrevAYA->setText(tr("الآية السابقة"));
        } else {
            m_actionNextAYA->setText(tr("انزال الصفحة")); // TODO: fix this tooltip?
            m_actionPrevAYA->setText(tr("رفع الصفحة"));
        }
    }
}

void BooksViewer::loadTafessirList()
{
    m_comboTafasir->clear();
    m_comboTafasir->setModel(m_taffesirManager->taffesirListModel());

    m_comboTafasir->setEditable(true);
    m_comboTafasir->completer()->setCompletionMode(QCompleter::PopupCompletion);

    updateToolBars();
}

void BooksViewer::editCurrentBook()
{
    LibraryBookPtr book = currentBook();
    BookPage *page = currentPage();

    if(book && page) {
        int pageID = page->pageID;

        try {
            MW->editorView()->editBook(book, pageID);
        } catch (BookException &e) {
            QMessageBox::information(this,
                                     App::name(),
                                     tr("حدث خطأ أثناء محاولة تحرير الكتاب الحالي:"
                                        "<br>%1").arg(e.what()));
        }
    }
}
