#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <qwebpage.h>

class WebPage : public QWebPage
{
    Q_OBJECT
public:
    WebPage(QObject *parent = 0);

protected:
    void javaScriptConsoleMessage ( const QString & message, int lineNumber, const QString & sourceID );
    bool acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, NavigationType type);

signals:
    void openMoltaqaLink(const QString &url);
};

#endif // WEBPAGE_H
