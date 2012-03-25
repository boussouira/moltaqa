#include "webpage.h"

WebPage::WebPage(QObject *parent) :
    QWebPage(parent)
{
}

void WebPage::javaScriptConsoleMessage ( const QString & message, int lineNumber, const QString & sourceID )
{
    qWarning("WebPage error: '%s', file: '%s', line: %d",
           qPrintable(message),
           qPrintable(sourceID),
           lineNumber);
}
