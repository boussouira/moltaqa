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
    m_textFormat->setData(m_bookInfo, m_currentPage);

    AbstractBookReader::connected();
}

bool RichBookReader::needFastIndexLoad()
{
    return true;
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

void RichBookReader::saveBookPage(QList<BookPage*> pages)
{
    QSqlQuery query(m_bookDB);

    m_bookDB.transaction();

    foreach(BookPage *page, pages) {
        qDebug("Save %d", page->pageID);
        //qDebug() << page->text;
        query.prepare("UPDATE bookPages SET "
                      "pageNum = ?, "
                      "partNum = ?, "
                      "pageText = ? "
                      "WHERE id = ?");
        query.bindValue(0, page->page);
        query.bindValue(1, page->part);
        query.bindValue(2, qCompress(page->text.toUtf8()));
        query.bindValue(3, page->pageID);

        if(!query.exec())
            LOG_SQL_ERROR(query);
    }

    if(m_bookDB.commit()) {
        qDebug("Committed");
    } else {
        qDebug("Not committed");
        LOG_DB_ERROR(m_bookDB);
    }
}

BookIndexModel *RichBookReader::topIndexModel()
{
    return new BookIndexModel();
}

TextFormatter *RichBookReader::textFormat()
{
    return m_textFormat;
}
