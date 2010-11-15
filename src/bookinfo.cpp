#include "bookinfo.h"
#include <qfile.h>

BookInfo::BookInfo()
{
}

void BookInfo::setFirstPage(int count, int part)
{
    if(part <= m_partsCount)
        m_firstPages.insert(part, count);
    else
        qWarning("The given part(%d) is out of range(%d)", part, m_partsCount);
}

int BookInfo::firstPage(int part)
{
    if(part <= m_partsCount)
        return m_firstPages.value(part);
    else {
        qWarning("The given part(%d) is out of range(%d)", part, m_partsCount);
        return 0;
    }
}

void BookInfo::setLastPage(int count, int part)
{
    if(part <= m_partsCount)
        m_lastPages.insert(part, count);
    else
        qWarning("The given part(%d) is out of range(%d)", part, m_partsCount);
}

int BookInfo::lastPage(int part)
{
    if(part <= m_partsCount)
        return m_lastPages.value(part);
    else {
        qWarning("The given part(%d) is out of range(%d)", part, m_partsCount);
        return 0;
    }
}

bool BookInfo::exists()
{
    if(!bookPath().isEmpty()){
        return QFile::exists(bookPath());
    } else {
        qWarning("Call to BookInfo::exists() before BookInfo::setBookPath()");
        return false;
    }
}
