#include "richbookreader.h"
#include "textformatter.h"
#include "bookpage.h"

RichBookReader::RichBookReader(QObject *parent) : AbstractBookReader(parent)
{
    m_textFormat = 0;
    m_query = 0;
    m_highlightPageID = -1;
}

RichBookReader::~RichBookReader()
{
    if(m_textFormat)
        delete m_textFormat;
}

bool RichBookReader::needFastIndexLoad()
{
    return true;
}

QString RichBookReader::text()
{
    Q_ASSERT(m_textFormat);

    if(m_query && m_highlightPageID == m_currentPage->pageID) {
        return Utils::highlightText(m_textFormat->getText(), m_query, false);
    } else {
        return m_textFormat->getText();
    }
}

void RichBookReader::highlightPage(int pageID, lucene::search::Query *query)
{
    m_query = query;
    m_highlightPageID = pageID;
}

QAbstractItemModel * RichBookReader::topIndexModel()
{
    return new QStandardItemModel();
}
