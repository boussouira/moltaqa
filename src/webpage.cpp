#include "webpage.h"
#include "webview.h"
#include "webpagenam.h"

#include <qdebug.h>
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
            QWebElement element = mainFrame()->findFirstElement('#' + url.fragment(QUrl::FullyDecoded));
            if(element.isNull())
                element = mainFrame()->findFirstElement("[name|=" + url.fragment(QUrl::FullyDecoded) + "]");

            if(!element.isNull()
                    && url.toString(QUrl::RemoveFragment) == mainFrame()->url().toString(QUrl::RemoveFragment)) {
                m_webView->scrollToPosition(element.geometry().topLeft(), -1);

                return false;
            }
        }
    }

    return QWebPage::acceptNavigationRequest(frame, request, type);
}
