#include "texttafessirreader.h"
#include "librarybook.h"
#include "utils.h"

TextTafessirReader::TextTafessirReader(QObject *parent) :
    TextBookReader(parent)
{
}

TextTafessirReader::~TextTafessirReader()
{
    m_titles.clear();
}

void TextTafessirReader::getTitles()
{
    QSqlQuery query(m_bookDB);

    query.prepare("SELECT pageID, id "
                  "FROM bookIndex "
                  "ORDER BY id");

    if(query.exec()) {
        while(query.next()) {
            m_titles.insert(query.value(0).toInt(), query.value(1).toInt());
        }
    } else {
        LOG_SQL_ERROR(query);
    }
}

void TextTafessirReader::goFirst()
{
    QString sql = "SELECT id, pageText, partNum, pageNum "
            "FROM bookPages "
            "ORDER BY id";

    if(!m_bookQuery.exec(sql))
        LOG_SQL_ERROR(m_bookQuery);
}

void TextTafessirReader::goToPage(int pid)
{
    Q_UNUSED(pid);
}

void TextTafessirReader::goToPage(int page, int part)
{
    Q_UNUSED(page);
    Q_UNUSED(part);
}

QString TextTafessirReader::text()
{
    return m_text;
}

bool TextTafessirReader::hasPrev()
{
    return false;
}

bool TextTafessirReader::hasNext()
{
    return m_bookQuery.next();
}

void TextTafessirReader::nextPage()
{
    m_text = QString::fromUtf8(qUncompress(m_bookQuery.value(1).toByteArray()));
    m_currentPage->pageID = m_bookQuery.value(0).toInt();
    m_currentPage->part = m_bookQuery.value(2).toInt();
    m_currentPage->page = m_bookQuery.value(3).toInt();

    m_currentPage->titleID = m_titles.value(m_currentPage->pageID,
                                            m_currentPage->titleID);
}
