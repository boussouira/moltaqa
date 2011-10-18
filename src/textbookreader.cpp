#include "textbookreader.h"
#include "librarybook.h"

TextBookReader::TextBookReader(QObject *parent) :
    AbstractBookReader(parent)
{
}

TextBookReader::~TextBookReader()
{
}

void TextBookReader::goFirst()
{
    m_bookQuery.exec(QString("SELECT id, pageText, partNum, pageNum from bookPages "
                             "ORDER BY id"));
}

void TextBookReader::goToPage(int pid)
{
    Q_UNUSED(pid);
}

void TextBookReader::goToPage(int page, int part)
{
    Q_UNUSED(page);
    Q_UNUSED(part);
}

QString TextBookReader::text()
{
    return m_text;
}

void TextBookReader::connected()
{
}

bool TextBookReader::hasPrev()
{
    return false;
}

bool TextBookReader::hasNext()
{
    return m_bookQuery.next();
}

void TextBookReader::nextPage()
{
    m_text = QString::fromUtf8(qUncompress(m_bookQuery.value(1).toByteArray()));
    m_currentPage->pageID = m_bookQuery.value(0).toInt();
    m_currentPage->part = m_bookQuery.value(2).toInt();
    m_currentPage->page = m_bookQuery.value(3).toInt();
}
