#include "resultwidget.h"
#include "ui_resultwidget.h"
#include "utils.h"
#include "webview.h"
#include "booksviewer.h"
#include "mainwindow.h"
#include "bookwidget.h"
#include <qdir.h>
#include <qplaintextedit.h>
#include <qboxlayout.h>
#include <qsettings.h>

ResultWidget::ResultWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ResultWidget)
{
    ui->setupUi(this);

    m_view = new WebView(this);
    ui->verticalLayout->insertWidget(0, m_view);

    connect(m_view->page()->mainFrame(),
            SIGNAL(javaScriptWindowObjectCleared()),
            SLOT(populateJavaScriptWindowObject()));

    setupWebView();
    setupBookReaderView();
}

ResultWidget::~ResultWidget()
{
    delete m_view;
    delete m_readerview;
    delete ui;
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
    m_readerview = new BooksViewer(MW->libraryManager());
    m_readerview->hideMenu();

    m_readerWidget = new QWidget(this);
    QWidget *toolBarWidget = new QWidget(this);
    QHBoxLayout *toolBarLayout = new QHBoxLayout();
    QVBoxLayout *mainlayout = new QVBoxLayout();

    foreach(QToolBar *bar, m_readerview->toolBars()) {
        toolBarLayout->addWidget(bar);
    }

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
    buttonHideBookView->setIcon(QIcon(":/menu/images/delete.png"));
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
    QDir styleDir(App::stylesDir());
    styleDir.cd("default");

    QString style = styleDir.filePath("default.css");
    QString  m_styleFile = QUrl::fromLocalFile(style).toString();

    QDir jsDir(App::jsDir());
    QString  m_jqueryGrowlFile = QUrl::fromLocalFile(jsDir.filePath("jquery.growl.js")).toString();
    QString  m_jqueryFile = QUrl::fromLocalFile(jsDir.filePath("jquery.js")).toString();
    QString  m_scriptFile = QUrl::fromLocalFile(jsDir.filePath("scripts.js")).toString();

    QString html = QString("<html>"
                           "<head>"
                           "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" />"
                           "<link href= \"%1\" rel=\"stylesheet\" type=\"text/css\" />"
                           "</head>"
                           "<body>"
                           "<div id=\"searchResult\">.</div>"
                           "<script type=\"text/javascript\" src=\"%2\"></script>"
                           "<script type=\"text/javascript\" src=\"%3\"></script>"
                           "<script type=\"text/javascript\" src=\"%4\"></script>"
                           "</body></html>").arg(m_styleFile, m_jqueryFile, m_jqueryGrowlFile, m_scriptFile);
    m_view->setHtml(html);
}

void ResultWidget::showNavigationButton(bool show)
{
    ui->progressWidget->setVisible(!show);
    ui->widgetNavigationButtons->setVisible(show);
}

void ResultWidget::updateNavigationInfo()
{
    int currentPage = m_searcher->currentPage();
    int pageCount = m_searcher->pageCount();

    int start = (currentPage * m_searcher->resultsPeerPage()) + 1 ;
    int end = qMax(1, (currentPage * m_searcher->resultsPeerPage()) + m_searcher->resultsPeerPage());

    end = (pageCount >= end) ? end : pageCount;
    ui->labelNav->setText(tr("%1 - %2 من %3 نتيجة")
                       .arg(start)
                       .arg(end)
                       .arg(pageCount));

    updateButtonStat();
}

void ResultWidget::updateButtonStat()
{
    bool back = (m_searcher->currentPage() > 0);
    bool next = (m_searcher->currentPage() < m_searcher->pageCount()-1);

    ui->buttonGoPrev->setEnabled(back);
    ui->buttonGoFirst->setEnabled(back);

    ui->buttonGoNext->setEnabled(next);
    ui->buttonGoLast->setEnabled(next);
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

void ResultWidget::openResult(int resultID)
{
    SearchResult *result = m_searcher->getResult(resultID);
    BookWidget *bookWidget = m_readerview->openBook(result->book->bookID,
                                                    result->page->pageID,
                                                    m_searcher->getSearchQuery());

    ensureReaderVisible();

    if(result->book->isQuran())
        bookWidget->openSora(result->page->sora, result->page->aya);
}

void ResultWidget::searchStarted()
{
    m_view->execJS("searchStarted();");

    ui->progressBar->setMaximum(0);
    showNavigationButton(false);
}

void ResultWidget::searchFinnished()
{
    m_view->execJS("searchFinnished();");
    m_view->execJS(QString("searchInfo(%1, %2);")
                   .arg(m_searcher->searchTime())
                   .arg(m_searcher->resultsCount()));
}

void ResultWidget::fetechStarted()
{
    m_view->execJS("fetechStarted();");
    showNavigationButton(false);

    QSettings settings;
    ui->progressBar->setMaximum(settings.value("Search/resultPeerPage", 10).toInt());
    ui->progressBar->setValue(0);
}

void ResultWidget::fetechFinnished()
{
    m_view->execJS("fetechFinnished();");
    updateNavigationInfo();
    showNavigationButton(true);

    //QPlainTextEdit *edit = new QPlainTextEdit(0);
    //edit->setPlainText(m_view->toHtml());
    //edit->show();
}

void ResultWidget::gotResult(SearchResult *result)
{
    result->generateHTML();
    m_view->execJS(QString("addResult('%1')").arg(result->toHtml()));

    ui->progressBar->setValue(ui->progressBar->value()+1);
}

void ResultWidget::gotException(QString what, int id)
{
    id++;
}

void ResultWidget::populateJavaScriptWindowObject()
{
    m_view->addObject("resultWidget", this);
}

void ResultWidget::on_buttonGoNext_clicked()
{
    m_searcher->nextPage();
}

void ResultWidget::on_buttonGoPrev_clicked()
{
    m_searcher->prevPage();
}

void ResultWidget::on_buttonGoLast_clicked()
{
    m_searcher->lastPage();
}

void ResultWidget::on_buttonGoFirst_clicked()
{
    m_searcher->firstPage();
}
