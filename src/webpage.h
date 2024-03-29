#ifndef WEBPAGE_H
#define WEBPAGE_H

#include "librarybook.h"

#include <qwebpage.h>

class WebView;
class WebPageNAM;

class WebPage : public QWebPage
{
    Q_OBJECT
public:
    WebPage(WebView *parent = 0);

    void setBook(LibraryBook::Ptr book);

protected:
    void javaScriptConsoleMessage ( const QString & message, int lineNumber, const QString & sourceID );
    bool acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, NavigationType type);
    QString chooseFile(QWebFrame * parentFrame, const QString &suggestedFile);
    QWebPage *createWindow(WebWindowType type);

signals:
    void openMoltaqaLink(const QString &url);

protected:
    WebView *m_webView;
    WebPageNAM *m_nam;
    LibraryBook::Ptr m_book;
};

#endif // WEBPAGE_H
