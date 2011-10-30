#include "textsimplebookreader.h"
#include "librarybook.h"
#include "utils.h"

TextSimpleBookReader::TextSimpleBookReader(QObject *parent) :
    TextBookReader(parent)
{
}

TextSimpleBookReader::~TextSimpleBookReader()
{
}

void TextSimpleBookReader::goFirst()
{
    QString sql = "SELECT id, pageText, partNum, pageNum "
            "FROM bookPages "
            "ORDER BY id";

    if(!m_bookQuery.exec(sql))
        LOG_SQL_ERROR(m_bookQuery);
}

void TextSimpleBookReader::goToPage(int pid)
{
    Q_UNUSED(pid);
}

void TextSimpleBookReader::goToPage(int page, int part)
{
    Q_UNUSED(page);
    Q_UNUSED(part);
}

QString TextSimpleBookReader::text()
{
    return m_text;
}

bool TextSimpleBookReader::hasPrev()
{
    return false;
}

bool TextSimpleBookReader::hasNext()
{
    return m_bookQuery.next();
}

void TextSimpleBookReader::nextPage()
{
    m_text = QString::fromUtf8(qUncompress(m_bookQuery.value(1).toByteArray()));
    m_currentPage->pageID = m_bookQuery.value(0).toInt();
    m_currentPage->part = m_bookQuery.value(2).toInt();
    m_currentPage->page = m_bookQuery.value(3).toInt();
}
