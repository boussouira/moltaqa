#include "resultwidget.h"
#include "ui_resultwidget.h"
#include "utils.h"
#include "webview.h"
#include "bookreaderview.h"
#include "librarybookmanager.h"
#include "bookwidget.h"
#include "htmlhelper.h"
#include "bookinfodialog.h"
#include "clconstants.h"
#include "mainwindow.h"
#include "richbookreader.h"
#include "stringutils.h"

#include <qdir.h>
#include <qplaintextedit.h>
#include <qboxlayout.h>
#include <qtoolbutton.h>
#include <qmenu.h>

ResultWidget::ResultWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ResultWidget)
{
    ui->setupUi(this);

    m_view = new WebView(this);
    ui->verticalLayout->insertWidget(0, m_view);
    ui->verticalLayout->setMargin(0);

    m_moveToReaderViewAct = new QAction(tr("نقل الى نافذة عرض الكتب"), this);

    connect(m_view->page()->mainFrame(),
            SIGNAL(javaScriptWindowObjectCleared()),
            SLOT(populateJavaScriptWindowObject()));

    connect(m_moveToReaderViewAct, SIGNAL(triggered()), SLOT(moveToReaderView()));

    setupWebView();
    setupBookReaderView();
}

ResultWidget::~ResultWidget()
{
    delete m_view;
    delete m_readerview;
    delete ui;
}

WebView *ResultWidget::resultWebView()
{
    return m_view;
}

void ResultWidget::search(LibrarySearcher *searcher)
{
    m_searcher = searcher;
    connect(m_searcher, SIGNAL(gotResult(SearchResult*)), SLOT(gotResult(SearchResult*)));
    connect(m_searcher, SIGNAL(startSearching()), SLOT(searchStarted()));
    connect(m_searcher, SIGNAL(doneSearching()), SLOT(searchFinnished()));
    connect(m_searcher, SIGNAL(startFeteching()), SLOT(fetechStarted()));
    connect(m_searcher, SIGNAL(doneFeteching()), SLOT(fetechFinnished()));
    connect(m_searcher, SIGNAL(gotException(QString, int)), SLOT(gotException(QString, int)));

    m_searcher->start();
}

void ResultWidget::setupBookReaderView()
{
    // Setup the book reader view
    m_readerview = new BookReaderView(LibraryManager::instance(), this);

    m_readerWidget = new QWidget(this);
    QWidget *toolBarWidget = new QWidget(this);
    QHBoxLayout *toolBarLayout = new QHBoxLayout();
    QVBoxLayout *mainlayout = new QVBoxLayout();

    foreach(QToolBar *bar, m_readerview->toolBars()) {
        toolBarLayout->addWidget(bar);
    }

    m_readerview->bookWidgetManager()->addTabActions(QList<QAction*>() << 0 << m_moveToReaderViewAct);

    // hide/maximize/minmize buttons
    QSpacerItem *horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    toolBarLayout->addItem(horizontalSpacer);

    QToolButton *buttonMinBookView = new QToolButton(this);
    buttonMinBookView->setArrowType(Qt::DownArrow);
    buttonMinBookView->setAutoRaise(true);

    toolBarLayout->addWidget(buttonMinBookView);

    QToolButton *buttonMaxBookView = new QToolButton(this);
    buttonMaxBookView->setArrowType(Qt::UpArrow);
    buttonMaxBookView->setAutoRaise(true);

    toolBarLayout->addWidget(buttonMaxBookView);

    QToolButton * buttonHideBookView = new QToolButton(this);
    buttonHideBookView->setIcon(QIcon(":/images/delete.png"));
    buttonHideBookView->setAutoRaise(true);

    toolBarLayout->addWidget(buttonHideBookView);

    toolBarLayout->setContentsMargins(9, 0, 9, 0);
    toolBarLayout->setSpacing(0);
    toolBarWidget->setLayout(toolBarLayout);

    mainlayout->setMenuBar(toolBarWidget);
    mainlayout->addWidget(m_readerview);

    m_readerWidget->setLayout(mainlayout);
    ui->splitter->addWidget(m_readerWidget);
    ensureReaderHidden(false);

    connect(m_readerview, SIGNAL(lastTabClosed()), SLOT(lastTabClosed()));
    connect(buttonHideBookView, SIGNAL(clicked()), SLOT(ensureReaderHidden()));
    connect(buttonMaxBookView, SIGNAL(clicked()), SLOT(maximizeBookReader()));
    connect(buttonMinBookView, SIGNAL(clicked()), SLOT(minimizeBookReader()));
}


void ResultWidget::setupWebView()
{
    HtmlHelper helper;
    helper.beginHtml();
    helper.beginHead();
    helper.setCharset("utf-8");
    helper.addCSS("default.css");
    helper.endHead();

    helper.beginBody();
    helper.insertDiv(".", "#searchResult");
    helper.insertDiv("", "#pagination");
    helper.addJS("jquery.js");
    helper.addJS("jquery.growl.js");
    helper.addJS("jquery.pagination.js");
    helper.addJS("scripts.js");
    helper.addJS("search.js");

    helper.endAll();

    m_view->setHtml(helper.html());
}

void ResultWidget::showProgressBar(bool show)
{
    ui->progressWidget->setVisible(show);
}

void ResultWidget::ensureReaderVisible()
{
    m_readerWidget->show();

    if(ui->splitter->sizes().at(1) == 0){
        QList<int> sizes;
        sizes << 200 << 300;
        ui->splitter->setSizes(sizes);
    }
}

void ResultWidget::ensureReaderHidden(bool accessible)
{
    QList<int> sizes;
    sizes << 200 << 0;
    ui->splitter->setSizes(sizes);

    m_readerWidget->setVisible(accessible);
}

void ResultWidget::maximizeBookReader()
{
    QList<int> sizes;
    sizes << 0;
    sizes << 100;

    ui->splitter->setSizes(sizes);
}

void ResultWidget::minimizeBookReader()
{
    QList<int> sizes;
    sizes << 200;
    sizes << 100;

    ui->splitter->setSizes(sizes);
}

void ResultWidget::lastTabClosed()
{
    ensureReaderHidden(false);
}

void ResultWidget::moveToReaderView()
{
    RichBookReader *reader = m_readerview->bookWidgetManager()->activeBookReader();
    ml_return_on_fail(reader);

    MW->bookReaderView()->openBook(reader->bookInfo()->id, reader->page()->pageID);
}

void ResultWidget::openResult(int resultID)
{
    SearchResult *result = m_searcher->getResult(resultID);
    BookWidget *bookWidget = m_readerview->openBook(result->book->id,
                                                    result->page->pageID,
                                                    m_searcher->getSearchQuery());

    if(bookWidget) {
        ensureReaderVisible();

        if(result->book->isQuran())
            bookWidget->openSora(result->page->sora, result->page->aya);
    }
}

void ResultWidget::goToPage(int page)
{
    m_searcher->fetechResults(page);
}

void ResultWidget::showBookMenu(int bookID)
{
    LibraryBook::Ptr book = LibraryManager::instance()->bookManager()->getLibraryBook(bookID);
    ml_return_on_fail2(book, "ResultWidget::showBookMenu no book with id" << bookID);

    QMenu menu(this);
    QAction *includeOnlyAct = new QAction(tr("بحث في هذا الكتاب فقط"), &menu);
    QAction *excludeAct = new QAction(tr("حذف هذا الكتاب من البحث"), &menu);
    QAction *bookInfoAct = new QAction(tr("بطاقة الكتاب"), &menu);

    menu.addAction(includeOnlyAct);
    menu.addAction(excludeAct);
    menu.addAction(bookInfoAct);


    QAction *ret = menu.exec(QCursor::pos());
    if(ret) {
        if(ret == includeOnlyAct || ret == excludeAct) {
            CLuceneQuery *query = m_searcher->getSearchQuery();

            wchar_t *idStr = Utils::CLucene::intToWChar(bookID);
            Term *term = new Term(BOOK_ID_FIELD, idStr);
            TermQuery *termQuery = new TermQuery(term);

            BooleanQuery *q = new BooleanQuery();
            q->add(termQuery, BooleanClause::SHOULD);

            if(ret == excludeAct && query->resultFilter) {
                if(query->resultFilter->clause == BooleanClause::MUST_NOT) {
                    // Add previous exclude filter to the new one
                    q->add(query->resultFilter->query, BooleanClause::SHOULD);

                    query->resultFilter->unSelected++;
                    query->resultFilter->selected = -1;
                } else {
                    query->resultFilter->selected = 1;
                    query->resultFilter->unSelected = 1;
                }
            }

            if(!query->resultFilter){
                query->resultFilter = new SearchFilter();
                query->resultFilter->selected = 1;
                query->resultFilter->unSelected = 1;
            }

            query->resultFilter->query = q;
            query->resultFilter->clause = ((ret == includeOnlyAct) ? BooleanClause::MUST : BooleanClause::MUST_NOT);

            m_searcher->setQuery(query);
            m_searcher->start();
        } else if(ret == bookInfoAct) {
            BookInfoDialog *dialog = new BookInfoDialog(0);
            dialog->setLibraryBook(book);
            dialog->setup();
            dialog->show();
        }
    }
}

void ResultWidget::searchStarted()
{
    m_view->execJS("searchStarted();");

    ui->progressBar->setMaximum(0);
    showProgressBar(true);
}

void ResultWidget::searchFinnished()
{
    m_view->execJS("searchFinnished();");
    m_view->execJS(QString("searchInfo(%1, %2);")
                   .arg(m_searcher->searchTime())
                   .arg(m_searcher->resultsCount()));

    showProgressBar(false);
}

void ResultWidget::fetechStarted()
{
    m_view->execJS("fetechStarted();");
    showProgressBar(true);

    ui->progressBar->setMaximum(Utils::Settings::get("Search/resultPeerPage", 10).toInt());
    ui->progressBar->setValue(0);
}

void ResultWidget::fetechFinnished()
{
    m_view->execJS("fetechFinnished();");
    m_view->execJS(QString("setPagination(%1, %2, %3)")
                   .arg(m_searcher->currentPage())
                   .arg(m_searcher->resultsCount())
                   .arg(m_searcher->resultsPeerPage()));

    showProgressBar(false);

    //QPlainTextEdit *edit = new QPlainTextEdit(0);
    //edit->setPlainText(m_view->toHtml());
    //edit->show();
}

void ResultWidget::gotResult(SearchResult *result)
{
    m_view->execJS(QString("addResult('%1')").arg(Utils::Html::jsEscape(result->toHtml())));

    ui->progressBar->setValue(ui->progressBar->value()+1);
}

void ResultWidget::gotException(QString what, int id)
{
    QString errorTitle;
    QString errorDesc;

    if(id == CL_ERR_TooManyClauses) {
        errorTitle = tr("احتمالات البحث كثيرة جدا");
        errorDesc = what;
    } else if(id == CL_ERR_IO){
        errorTitle = tr("لم تقم بانشاء الفهرس بعد");
        errorDesc = what;
    } else if(id == CL_ERR_CorruptIndex){
        errorTitle = tr("الفهرس غير سليم");
        errorDesc = what;
    } else {
        errorTitle = tr("خطأ غير معروف");
        errorDesc = what;
    }

    m_view->execJS(QString("showError('%1', '%2');")
                   .arg(Utils::Html::jsEscape(errorTitle))
                   .arg(Utils::Html::jsEscape(errorDesc)));

    showProgressBar(false);
}

void ResultWidget::populateJavaScriptWindowObject()
{
    m_view->addObject("resultWidget", this);
}
