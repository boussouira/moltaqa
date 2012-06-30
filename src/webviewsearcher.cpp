#include "webviewsearcher.h"
#include "stringutils.h"
#include "utils.h"

#include <qwebview.h>
#include <qwebframe.h>

WebViewSearcher::WebViewSearcher(QWebView *view) :
    m_webView(view),
    m_currentMatch(-1)
{
}

void WebViewSearcher::setSearchText(QString text)
{
    clear();
    m_searchText = text.trimmed();
}

void WebViewSearcher::setWebView(QWebView *view)
{
    clear();
    m_webView = view;
}

void WebViewSearcher::clear()
{
    m_searchText.clear();
    m_currentMatch = -1;
    m_matches.clear();
}

bool WebViewSearcher::search(bool goNext)
{
    m_matches = Utils::String::Arabic::getMatchString(m_webView->page()->mainFrame()->toPlainText(),
                                                      m_searchText);
    m_currentMatch = -1;

    if(m_matches.size() && goNext)
        next();

    return m_matches.size();
}

bool WebViewSearcher::next()
{
    ml_return_val_on_fail(m_matches.size(), false);

    m_currentMatch = (hasNext() ? m_currentMatch+1 : 0);

    return m_webView->findText(m_matches[m_currentMatch],
                               QWebPage::FindWrapsAroundDocument);
}

bool WebViewSearcher::previous()
{
    ml_return_val_on_fail(m_matches.size(), false);

    m_currentMatch = (hasPrevious() ? m_currentMatch-1 : m_matches.size()-1);

    return m_webView->findText(m_matches[m_currentMatch],
            QWebPage::FindWrapsAroundDocument|QWebPage::FindBackward);
}

bool WebViewSearcher::hasSearchResult()
{
    return m_matches.size();
}

bool WebViewSearcher::hasNext()
{
    return m_currentMatch+1 < m_matches.size();
}

bool WebViewSearcher::hasPrevious()
{
    return m_currentMatch-1 >= 0;
}
