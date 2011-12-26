#include "textquranreader.h"
#include "bookpage.h"
#include "utils.h"

TextQuranReader::TextQuranReader(QObject *parent) :
    TextBookReader(parent)
{
}

void TextQuranReader::goToPage(int pid)
{
    Q_UNUSED(pid);
}

void TextQuranReader::goToPage(int page, int part)
{
    Q_UNUSED(page);
    Q_UNUSED(part);
}

bool TextQuranReader::hasPrev()
{
    return false;
}

bool TextQuranReader::hasNext()
{
    return 0;
    return m_bookQuery.next();
}

void TextQuranReader::nextPage()
{
    m_currentPage->text = m_bookQuery.value(1).toString();
    m_currentPage->pageID = m_bookQuery.value(0).toInt();
    m_currentPage->aya = m_bookQuery.value(2).toInt();
    m_currentPage->sora = m_bookQuery.value(3).toInt();
    m_currentPage->page = m_bookQuery.value(4).toInt();
    m_currentPage->part = 1;
}
