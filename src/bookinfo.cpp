#include "bookinfo.h"

BookInfo::BookInfo()
{
}

void BookInfo::setFirstPage(int count, int part)
{
    if(part <= m_partsCount)
        m_firstPages.insert(part, count);
    else
        qDebug("The given part is out of range");
}

int BookInfo::firstPage(int part)
{
    if(part <= m_partsCount)
        return m_firstPages.value(part);
    else {
        qDebug("The given part is out of range");
        return 0;
    }
}

void BookInfo::setLastPage(int count, int part)
{
    if(part <= m_partsCount)
        m_lastPages.insert(part, count);
    else
        qDebug("The given part is out of range");
}

int BookInfo::lastPage(int part)
{
    if(part <= m_partsCount)
        return m_lastPages.value(part);
    else {
        qDebug("The given part is out of range");
        return 0;
    }
}

