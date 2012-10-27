#include "webpage.h"
#include "webview.h"

#include <qdebug.h>
#include <qnetworkrequest.h>
#include <qwebelement.h>

WebPage::WebPage(QObject *parent) :
    QWebPage(parent)
{
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
        emit openMoltaqaLink(url.toString());

        return false;
    } else if(url.hasFragment()){
        WebView *webView = qobject_cast<WebView*>(parent());
        if(webView) {
            QWebElement element = mainFrame()->findFirstElement('#' + url.encodedFragment());
            if(element.isNull())
                element = mainFrame()->findFirstElement("[name|=" + url.encodedFragment() + "]");

            if(!element.isNull()
                    && url.toString(QUrl::RemoveFragment) == mainFrame()->url().toString(QUrl::RemoveFragment)) {
                webView->scrollToPosition(element.geometry().topLeft(), -1);

                return false;
            }
        }
    }

    return QWebPage::acceptNavigationRequest(frame, request, type);
}
