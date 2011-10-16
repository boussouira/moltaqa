#include "textbookreader.h"
#include "librarybook.h"

TextBookReader::TextBookReader(QObject *parent) :
    AbstractBookReader(parent)
{
    m_simpleQuery = 0;
}

TextBookReader::~TextBookReader()
{
    if(m_simpleQuery)
        delete m_simpleQuery;
}

void TextBookReader::goToPage(int pid)
{
    int id;
    if(pid == -1) // First page
        id = m_bookInfo->firstID;
    else if(pid == -2) //Last page
        id = m_bookInfo->lastID;
    else // The given page id
        id = pid;

    if(id >= m_bookInfo->currentPage.pageID)
        m_simpleQuery->nextPage(id);
    else
        m_simpleQuery->prevPage(id);

    if(m_simpleQuery->next()) {
        m_text = QString::fromUtf8(qUncompress(m_simpleQuery->value(1).toByteArray()));
        m_bookInfo->currentPage.pageID = m_simpleQuery->value(0).toInt();
        m_bookInfo->currentPage.part = m_simpleQuery->value(2).toInt();
        m_bookInfo->currentPage.page = m_simpleQuery->value(3).toInt();
    }

    m_libraryManager->getShoroohPages(m_bookInfo);
}

void TextBookReader::goToPage(int page, int part)
{
    m_simpleQuery->page(page, part);
    if(m_simpleQuery->next())
        goToPage(m_simpleQuery->value(0).toInt());
}

QString TextBookReader::text()
{
    return m_text;
}

void TextBookReader::connected()
{
    m_simpleQuery = new SimpleQuery(m_bookDB, m_bookInfo);
}
