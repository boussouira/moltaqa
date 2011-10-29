#include "resultwidget.h"
#include "ui_resultwidget.h"
#include "utils.h"
#include "webview.h"
#include <qdir.h>
#include <QPlainTextEdit>

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

    initWebView();
}

ResultWidget::~ResultWidget()
{
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


void ResultWidget::initWebView()
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
                           "<script type=\"text/javascript\" src=\"%2\" />"
                           "<script type=\"text/javascript\" src=\"%3\" />"
                           "<script type=\"text/javascript\" src=\"%4\" />"
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

void ResultWidget::openResult(int resultID)
{
    qDebug("ID: %d", resultID);
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
    m_view->execJS(QString("searchInfo('%1', '%2');")
                   .arg(m_searcher->searchTime())
                   .arg(m_searcher->resultsCount()));
}

void ResultWidget::fetechStarted()
{
    m_view->execJS("fetechStarted();");
    showNavigationButton(false);
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
