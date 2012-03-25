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
};

#endif // WEBPAGE_H
