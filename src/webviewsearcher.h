#ifndef WEBVIEWSEARCHER_H
#define WEBVIEWSEARCHER_H

#include <qdialog.h>

class FilterLineEdit;
class WebView;
class QPushButton;
class QLabel;

class WebViewSearcher : public QDialog
{
    Q_OBJECT

public:
    WebViewSearcher(WebView *view=0);

    void setSearchText(const QString &text);
    void setWebView(WebView *view);
    void clear();

    bool hasSearchResult();
    bool hasNext();
    bool hasPrevious();

public slots:
    bool search();
    bool search(const QString &text);

    bool searchNext();
    bool searchPrevious();
    void searchAll();

protected:
    FilterLineEdit *m_searchLine;
    QPushButton *m_nextButton;
    QPushButton *m_prevButton;
    QPushButton *m_allButton;
    QLabel *m_resultLabel;
    QString m_searchText;
    WebView *m_webView;
    QStringList m_matches;
    int m_currentMatch;
};

#endif // WEBVIEWSEARCHER_H
