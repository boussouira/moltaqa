#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <qwebpage.h>
#include "librarybook.h"

class WebView;
class WebPageNAM;

class WebPage : public QWebPage
{
    Q_OBJECT
public:
    WebPage(WebView *parent = 0);

    void setBook(LibraryBookPtr book);

protected:
    void javaScriptConsoleMessage ( const QString & message, int lineNumber, const QString & sourceID );
    bool acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, NavigationType type);

signals:
    void openMoltaqaLink(const QString &url);

protected:
    WebView *m_webView;
    WebPageNAM *m_nam;
    LibraryBookPtr m_book;
};

#endif // WEBPAGE_H
