#include "welcomewidget.h"
#include "authorsmanager.h"
#include "booklistmanager.h"
#include "favouritesmanager.h"
#include "htmlhelper.h"
#include "librarybookmanager.h"
#include "librarymanager.h"
#include "mainwindow.h"
#include "modelenums.h"
#include "modelutils.h"
#include "modelviewfilter.h"
#include "tarajemrowatmanager.h"
#include "utils.h"
#include "webpage.h"
#include "webview.h"

#include <qboxlayout.h>
#include <qnetworkcookiejar.h>
#include <qmenu.h>
#include <qrunnable.h>
#include <qsettings.h>
#include <qtimer.h>
#include <qurl.h>
#include <qwebframe.h>

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
    m_webView->autoObjectAdd("welcome", this);

    layout->addWidget(m_webView);
    setLayout(layout);

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

void WelcomeWidget::setNetworkCookieJar(QNetworkCookieJar *cookieJar)
{
    m_webView->page()->networkAccessManager()->setCookieJar(cookieJar);
    cookieJar->setParent(0);
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
        MW->showBooksList(BooksListBrowser::CategoriesModel);
    } else if(vid == "moltaqa-lib") {
        m_webView->setUrl(QUrl("http://www.ahlalhdeeth.com/vb/forumdisplay.php?f=75"));
    } else {
        qWarning() << "WelcomeWidget::open unknow id" << vid;
    }
}

QString WelcomeWidget::getFontFamily()
{
    QSettings settings;
    settings.beginGroup("DefaultFont");

    return QString("'%1'").arg(settings.value("fontFamily").toString());
}

QString WelcomeWidget::getFontSize()
{
    QSettings settings;
    settings.beginGroup("DefaultFont");

    return QString("%1px").arg(settings.value("fontSize").toInt());
}

void WelcomeWidget::showStatistics()
{
    QThreadPool::globalInstance()->start(new LibraryInfoThread(m_webView));
}
