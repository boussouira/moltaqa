#include "webpage.h"
#include "webpagenam.h"
#include "webview.h"

#include <qaction.h>
#include <qboxlayout.h>
#include <qfiledialog.h>
#include <qnetworkrequest.h>
#include <qwebelement.h>

WebPage::WebPage(WebView *parent) :
    QWebPage(parent)
{
    m_webView = parent;
    m_nam = new WebPageNAM(this);

    setNetworkAccessManager(m_nam);
}

void WebPage::setBook(LibraryBook::Ptr book)
{
    m_book = book;
    m_nam->setBook(book);
}

void WebPage::javaScriptConsoleMessage ( const QString & message, int lineNumber, const QString & sourceID )
{
    qWarning("WebPage: javaScript error '%s', file: '%s', line: %d",
           qPrintable(message),
           qPrintable(sourceID),
           lineNumber);
}

bool WebPage::acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, NavigationType type)
{
    QUrl url = request.url();

    if(url.scheme() == "moltaqa") {

#ifdef DEV_BUILD
        qDebug() << qPrintable(QString("WebPage: openMoltaqaLink(%1)").arg(url.toString()));
#endif

        emit openMoltaqaLink(url.toString());

        return false;
    } else if(url.hasFragment()){
        if(m_webView) {
            QWebElement element = mainFrame()->findFirstElement('#' + url.encodedFragment());
            if(element.isNull())
                element = mainFrame()->findFirstElement("[name|=" + url.encodedFragment() + "]");

            if(!element.isNull()
                    && url.toString(QUrl::RemoveFragment) == mainFrame()->url().toString(QUrl::RemoveFragment)) {
                m_webView->scrollToPosition(element.geometry().topLeft(), -1);

                return false;
            }
        }
    }

    return QWebPage::acceptNavigationRequest(frame, request, type);
}

QString WebPage::chooseFile(QWebFrame *parentFrame, const QString &suggestedFile)
{
    Q_UNUSED(parentFrame);

    QString fileName = QFileDialog::getOpenFileName(0, tr("Open File"),
                                                    suggestedFile,
                                                    tr("All files (*.*)"));

    return fileName;
}

QWebPage *WebPage::createWindow(QWebPage::WebWindowType type)
{
    // Try to get the sender WebView
    WebView *senderView = 0;
    QAction *senderAction = qobject_cast<QAction*>(sender());
    if(senderAction && senderAction->parent()) {
        QWebPage *senderPage = qobject_cast<QWebPage*>(senderAction->parent());
        if(senderPage)
            senderView = qobject_cast<WebView*>(senderPage->view());
    }

    QWidget *widget = new QWidget(0);
    widget->setWindowTitle(tr("جاري التحميل..."));

    if(type == WebModalDialog)
        widget->setWindowModality(Qt::ApplicationModal);

    WebView *webView = new WebView(widget);
    if(senderView)
        webView->setBook(senderView->getLibraryBook());

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->addWidget(webView);

    widget->setLayout(layout);
    widget->show();

    connect(webView, SIGNAL(titleChanged(QString)), widget, SLOT(setWindowTitle(QString)));

    return webView->page();
}
