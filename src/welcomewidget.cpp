#include "welcomewidget.h"
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
#include "webview.h"
#include "webpage.h"

#include <qsettings.h>
#include <qmenu.h>
#include <qurl.h>
#include <qwebframe.h>
#include <qrunnable.h>
#include <qtimer.h>
#include <QVBoxLayout>

class LibraryInfoThread : public QRunnable {
public:
    LibraryInfoThread(WebView *_view) :
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

    WebView *view;
    int booksCount;
    int authorsCount;
    int rowatCount;
};

WelcomeWidget::WelcomeWidget(QWidget *parent) :
    AbstarctView(parent),
    m_bookManager(LibraryManager::instance()->bookManager())
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);

    m_webView = new WebView(this);
    layout->addWidget(m_webView);

    setLayout(layout);

    connect(m_webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
            SLOT(jsCleared()));

    setupHTML();
}

WelcomeWidget::~WelcomeWidget()
{
    saveSettings();
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

    m_webView->setUrl(QUrl::fromLocalFile(dir.absoluteFilePath("welcome.html")));
}

void WelcomeWidget::open(QString vid)
{
    vid = vid.toLower();

    if(vid == "quran") {
        LibraryBook::Ptr quran = m_bookManager->getQuranBook();
        if(quran)
            MW->openBook(quran->id);
        else
            qWarning("WelcomeWidget::open quran not found");
    } else if(vid == "search") {
        MW->showSearchView();
    } else if(vid == "lastbooks") {
        MW->showBooksList(BooksListBrowser::LastOpenModel);
    } else if(vid == "favourites") {
        MW->showBooksList(BooksListBrowser::FavoritesModel);
    } else if(vid == "bookslist") {
        MW->showBooksList(BooksListBrowser::AllBooksModel);
    } else if(vid == "moltaqa-lib") {
        m_webView->setUrl(QUrl("http://www.ahlalhdeeth.com/vb/forumdisplay.php?f=75"));
    } else {
        qWarning() << "WelcomeWidget::open unknow id" << vid;
    }
}

void WelcomeWidget::jsCleared()
{
    m_webView->page()->mainFrame()->addToJavaScriptWindowObject("welcome", this);
}

void WelcomeWidget::showStatistics()
{
    QThreadPool::globalInstance()->start(new LibraryInfoThread(m_webView));
}

