#include "webviewsearcher.h"
#include "filterlineedit.h"
#include "stringutils.h"
#include "utils.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qwebframe.h>
#include <qwebview.h>

WebViewSearcher::WebViewSearcher(QWebView *view) :
    QDialog(view),
    m_webView(view),
    m_currentMatch(-1)
{
    setWindowTitle(tr("بحث"));

    QVBoxLayout *layout = new QVBoxLayout(this);

    QHBoxLayout *labelLayout = new QHBoxLayout();
    QLabel *label = new QLabel(tr("بحث عن:"), this);
    m_searchLine = new FilterLineEdit(this);

    labelLayout->addWidget(label);
    labelLayout->addWidget(m_searchLine);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_nextButton = new QPushButton(tr("التالي"), this);
    m_prevButton = new QPushButton(tr("السابق"), this);
    m_allButton = new QPushButton(tr("الكل"), this);

    buttonLayout->addWidget(m_nextButton);
    buttonLayout->addWidget(m_prevButton);
    buttonLayout->addWidget(m_allButton);
    buttonLayout->addStretch();

    m_resultLabel = new QLabel(this);

    layout->addLayout(labelLayout);
    layout->addWidget(m_resultLabel);
    layout->addStretch();
    layout->addLayout(buttonLayout);

    connect(m_searchLine, SIGNAL(returnPressed()), SLOT(search()));
    connect(m_searchLine, SIGNAL(delayFilterChanged()), SLOT(search()));
    connect(m_nextButton, SIGNAL(clicked()), SLOT(searchNext()));
    connect(m_prevButton, SIGNAL(clicked()), SLOT(searchPrevious()));
    connect(m_allButton, SIGNAL(clicked()), SLOT(searchAll()));
}

void WebViewSearcher::setSearchText(const QString &text)
{
    clear();
    m_searchLine->setText(text);
    m_searchText = text;
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

bool WebViewSearcher::search()
{
    clear();

    m_searchText = m_searchLine->text();
    m_matches = Utils::String::Arabic::getMatchString(m_webView->page()->mainFrame()->toPlainText(),
                                                      m_searchText);
    m_currentMatch = -1;

    if(m_matches.size())
        searchNext();

    m_resultLabel->setText(tr("عدد النتائج: %1").arg(m_matches.size()));

    return m_matches.size();
}

bool WebViewSearcher::search(const QString &text)
{
    setSearchText(text);
    return search();
}

bool WebViewSearcher::searchNext()
{
    ml_return_val_on_fail(m_matches.size(), false);

    m_currentMatch = (hasNext() ? m_currentMatch+1 : 0);

    return m_webView->findText(m_matches[m_currentMatch],
                               QWebPage::FindWrapsAroundDocument);
}

bool WebViewSearcher::searchPrevious()
{
    ml_return_val_on_fail(m_matches.size(), false);

    m_currentMatch = (hasPrevious() ? m_currentMatch-1 : m_matches.size()-1);

    return m_webView->findText(m_matches[m_currentMatch],
                               QWebPage::FindWrapsAroundDocument|QWebPage::FindBackward);
}

void WebViewSearcher::searchAll()
{
    if(m_matches.isEmpty()) {
        m_webView->findText("", QWebPage::HighlightAllOccurrences);
    } else {
        foreach (QString match, m_matches)
            m_webView->findText(match, QWebPage::HighlightAllOccurrences);
    }
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
