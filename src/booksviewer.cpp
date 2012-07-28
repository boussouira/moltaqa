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
#include "filterlineedit.h"
#include "webview.h"

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

BooksViewer::BooksViewer(LibraryManager *libraryManager, QWidget *parent): AbstarctView(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    m_viewManager = new BookWidgetManager(this);

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
    m_actionNewTab = new QAction(QIcon::fromTheme("tab-new", QIcon(":/images/tab-new.png")),
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
    m_actionGotToPage = new QAction(QIcon::fromTheme("go-jump", QIcon(":/images/go-jump.png")),
                                    tr("انتقل الى..."),
                                    this);

    m_bookInfoAct = new QAction(tr("بطاقة الكتاب"), this);
    QAction *readHistoryAct = new QAction(tr("تاريخ تصفح الكتاب"), this);

    m_removeTashekilAct = new QAction(tr("حذف التشكيل"), this);
    m_removeTashekilAct->setCheckable(true);
    m_removeTashekilAct->setChecked(Utils::Settings::get("Style/removeTashekil", false).toBool());

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

    // Search widgets
    m_searchEdit = new FilterLineEdit(this);
    m_searchEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    m_searchEdit->setToolTip(tr("بحث في الصفحة"));

    m_searchPrevAction =  new QAction(ml_theme_icon("go-up-search", ":/images/go-up-search.png"),
                                      tr("السابق"),
                                      this);
    m_searchNextAction =  new QAction(ml_theme_icon("go-down-search", ":/images/go-down-search.png"),
                                      tr("التالي"),
                                      this);

    m_searchNextAction->setVisible(false);
    m_searchPrevAction->setVisible(false);

    connect(m_searchEdit, SIGNAL(delayFilterChanged()), SLOT(searchInPage()));
    connect(m_searchNextAction, SIGNAL(triggered()), SLOT(searchNext()));
    connect(m_searchPrevAction, SIGNAL(triggered()), SLOT(searchPrev()));

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
    m_toolBarNavigation->addAction(m_actionGotToPage);

    m_toolBarTafesir = new QToolBar(tr("التفاسير"), this);
    m_toolBarTafesir->addWidget(m_comboTafasir);
    m_toolBarTafesir->addAction(m_openSelectedTafsir);

    m_toolBarSearch = new QToolBar(tr("البحث"), this);
    m_toolBarSearch->addWidget(m_searchEdit);
    m_toolBarSearch->addAction(m_searchNextAction);
    m_toolBarSearch->addAction(m_searchPrevAction);

    updateSearchNavigation();

    m_navActions << m_actionEditBook;
    m_navActions << actionSeparator(this);
    m_navActions << m_actionFirstPage;
    m_navActions << m_actionPrevPage;
    m_navActions << m_actionNextPage;
    m_navActions << m_actionLastPage;
    m_navActions << m_actionGotToPage;
    m_navActions << actionSeparator(this);
    m_navActions << m_removeTashekilAct;
    m_navActions << actionSeparator(this);
    m_navActions << m_bookInfoAct;
    m_navActions << readHistoryAct;

    m_toolBars << m_toolBarGeneral;
    m_toolBars << m_toolBarNavigation;
    m_toolBars << m_toolBarTafesir;
    m_toolBars << m_toolBarSearch;

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
    connect(m_bookInfoAct, SIGNAL(triggered()), m_viewManager, SLOT(showBookInfo()));
    connect(readHistoryAct, SIGNAL(triggered()), m_viewManager, SLOT(showBookHistory()));
    connect(m_removeTashekilAct, SIGNAL(triggered(bool)), SLOT(removeTashkil(bool)));

    // Generale actions
    connect(m_actionIndexDock, SIGNAL(triggered()), SLOT(showIndexWidget()));
    connect(m_actionNewTab, SIGNAL(triggered()), MW, SLOT(showBooksList()));
    connect(m_actionSearchInBook, SIGNAL(triggered()), this, SLOT(searchInBook()));

    // Tafessir actions
    connect(m_openSelectedTafsir, SIGNAL(triggered()), SLOT(openTafessir()));
    connect(m_taffesirManager, SIGNAL(ModelsReady()), SLOT(loadTafessirList()));
}

void BooksViewer::updateSearchNavigation()
{
    ml_return_on_fail(currentBookWidget());

    m_searchNextAction->setVisible(currentBookWidget()->webView()->searcher()->hasSearchResult());
    m_searchPrevAction->setVisible(currentBookWidget()->webView()->searcher()->hasSearchResult());
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

        updateSearchNavigation();
    }
}

QString BooksViewer::viewLink()
{
    RichBookReader *bookReader = m_viewManager->activeBookReader();
    ml_return_val_on_fail(bookReader, QString());

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

BookWidget *BooksViewer::currentBookWidget()
{
    return m_viewManager->activeBookWidget();
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

        emit showMe();

    } catch (BookException &e) {
        QMessageBox::critical(this,
                              tr("فتح كتاب"),
                              e.what());

        ml_delete_check(bookReader);
        ml_delete_check(bookWidget);
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
        ml_return_on_fail(bookInfo && bookInfo->isTafessir() && m_viewManager->activeBook()->isQuran());

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
        ml_delete_check(bookdb);
        ml_delete_check(bookWidget);

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
    ml_return_on_fail(book);

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

void BooksViewer::searchInPage()
{
    ml_return_on_fail(currentBookWidget());

    QString searchText = m_searchEdit->text().trimmed();
    bool hasResult = currentBookWidget()->search(searchText);

    QString bg = ((searchText.isEmpty() || hasResult) ? "#FFFFFF" : "#F2DEDE");
    m_searchEdit->setStyleSheet(QString("background-color: %1").arg(bg));

    updateSearchNavigation();
}

void BooksViewer::searchNext()
{
    ml_return_on_fail(currentBookWidget());

    currentBookWidget()->searchNext();
    updateSearchNavigation();
}

void BooksViewer::searchPrev()
{
    ml_return_on_fail(currentBookWidget());

    currentBookWidget()->searchPrevious();
    updateSearchNavigation();
}

void BooksViewer::removeTashkil(bool remove)
{
    Utils::Settings::set("Style/removeTashekil", remove);

    QList<BookWidget *> list = m_viewManager->getBookWidgets();
    foreach (BookWidget *book, list) {
        book->bookReader()->setRemoveTashkil(remove);
        book->reloadCurrentPage();
    }
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
