#ifndef WEBVIEWSEARCHER_H
#define WEBVIEWSEARCHER_H

#include <qstringlist.h>

class QWebView;

class WebViewSearcher
{
public:
    WebViewSearcher(QWebView *view=0);

    void setSearchText(const QString &text);
    void setWebView(QWebView *view);
    void clear();

    bool search(bool goNext=true);
    bool search(const QString &text, bool goNext=true);
    bool next();
    bool previous();

    bool hasSearchResult();
    bool hasNext();
    bool hasPrevious();

protected:
    QString m_searchText;
    QWebView *m_webView;
    QStringList m_matches;
    int m_currentMatch;
};

#endif // WEBVIEWSEARCHER_H
