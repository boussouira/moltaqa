#include "richbookreader.h"
#include "textformatter.h"
#include "librarybook.h"
#include "bookpage.h"
#include "bookindexmodel.h"
#include "utils.h"

RichBookReader::RichBookReader(QObject *parent) : AbstractBookReader(parent)
{
    m_textFormat = 0;
    m_query = 0;
    m_highlightPageID = -1;
    m_stopModelLoad = false;
}

RichBookReader::~RichBookReader()
{
    if(m_textFormat)
        delete m_textFormat;
}

void RichBookReader::connected()
{
    if(!m_bookInfo->isQuran())
        m_textFormat->setData(m_bookInfo, m_currentPage);

    AbstractBookReader::connected();
}

bool RichBookReader::needFastIndexLoad()
{
    return true;
}

QString RichBookReader::text()
{
    Q_ASSERT(m_textFormat);

    return m_textFormat->getText();
}

void RichBookReader::highlightPage(int pageID, lucene::search::Query *query)
{
    m_query = query;
    m_highlightPageID = pageID;
}

bool RichBookReader::scrollToHighlight()
{
    return (!m_bookInfo->isQuran() && m_query && m_highlightPageID == m_currentPage->pageID);
}

void RichBookReader::stopModelLoad()
{
    m_stopModelLoad = true;
}

int RichBookReader::getPageTitleID(int pageID)
{
    QSqlQuery query(m_bookDB);

    query.prepare(QString("SELECT pageID FROM %1 "
                          "WHERE pageID <= ? "
                          "ORDER BY pageID DESC LIMIT 1")
                  .arg(m_bookInfo->indexTable));

    query.bindValue(0, pageID);
    if(query.exec()) {
        if(query.next()) {
            return query.value(0).toInt();
        }
    } else {
        LOG_SQL_ERROR(query);
    }

    return 0;
}

BookIndexModel *RichBookReader::topIndexModel()
{
    return new BookIndexModel();
}
