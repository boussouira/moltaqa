#include "bookinfo.h"

BookInfo::BookInfo()
{
}

void BookInfo::setPagesCount(int count, int part)
{
    if(part <= m_partsCount)
        m_pagesCount.insert(part, count);
    else
        qDebug("The given part is out of range");
}

int BookInfo::pagesCount(int part)
{
    if(part <= m_partsCount)
        return m_pagesCount.value(part);
    else {
        qDebug("The given part is out of range");
        return 0;
    }
}
