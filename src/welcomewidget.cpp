#include "welcomewidget.h"
#include "ui_welcomewidget.h"
#include "librarymanager.h"
#include "modelenums.h"
#include "modelutils.h"
#include "mainwindow.h"
#include "booklistmanager.h"
#include "favouritesmanager.h"
#include "modelviewfilter.h"
#include "librarybookmanager.h"
#include "utils.h"
#include "htmlhelper.h"
#include "authorsmanager.h"
#include "tarajemrowatmanager.h"

#include <qsettings.h>
#include <qmenu.h>
#include <qurl.h>
#include <qwebframe.h>
#include <qrunnable.h>
#include <qtimer.h>
#include "webpage.h"

class LibraryInfoThread : public QRunnable {
public:
    LibraryInfoThread(QWebView *_view) :
        view(_view),
        booksCount(0),
        authorsCount(0),
        rowatCount(0) {}

    void run()
    {
        QHash<QString, QVariant> s = LibraryManager::instance()->libraryStatistic();
        booksCount = s["books_count"].toInt();
        authorsCount = s["authors_count"].toInt();
        rowatCount = s["rowat_count"].toInt();

        QMetaObject::invokeMethod(view->page()->mainFrame(), "evaluateJavaScript",
                                  Q_ARG(QString, QString("showStatistics(%1, %2, %3)")
                                  .arg(booksCount)
                                  .arg(authorsCount)
                                  .arg(rowatCount)));
    }

    QWebView *view;
    int booksCount;
    int authorsCount;
    int rowatCount;
};

WelcomeWidget::WelcomeWidget(QWidget *parent) :
    AbstarctView(parent),
    m_bookManager(LibraryManager::instance()->bookManager()),
    ui(new Ui::WelcomeWidget)
{
    ui->setupUi(this);

    ui->webView->setPage(new WebPage(ui->webView));

    connect(ui->webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
            SLOT(jsCleared()));

    setupHTML();
}

WelcomeWidget::~WelcomeWidget()
{
    saveSettings();
    delete ui;
}

QString WelcomeWidget::title()
{
    return tr("صفحة البداية");
}

void WelcomeWidget::loadSettings()
{
}

void WelcomeWidget::saveSettings()
{
}

void WelcomeWidget::setupHTML()
{
    QDir dir(App::dataDir());
    dir.cd("welcome");

    ui->webView->setUrl(QUrl::fromLocalFile(dir.absoluteFilePath("welcome.html")));
}

void WelcomeWidget::open(QString vid)
{
    vid = vid.toLower();

    if(vid == "quran") {
        LibraryBookPtr quran = m_bookManager->getQuranBook();
        if(quran)
            MW->openBook(quran->id);
        else
            qWarning("WelcomeWidget::open quran not found");
    } else if(vid == "search") {
        MW->showSearchView();
    } else if(vid == "lastbooks") {
        MW->showBooksList(2);
    } else if(vid == "favourites") {
        MW->showBooksList(1);
    } else if(vid == "bookslist") {
        MW->showBooksList(0);
    } else if(vid == "moltaqa-lib") {
        ui->webView->setUrl(QUrl("http://www.ahlalhdeeth.com/vb/forumdisplay.php?f=75"));
    } else {
        qWarning() << "WelcomeWidget::open unknow id" << vid;
    }
}

void WelcomeWidget::jsCleared()
{
    ui->webView->page()->mainFrame()->addToJavaScriptWindowObject("welcome", this);
}

void WelcomeWidget::showStatistics()
{
    QThreadPool::globalInstance()->start(new LibraryInfoThread(ui->webView));
}

