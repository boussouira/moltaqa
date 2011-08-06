#include "bookinfo.h"
#include <qfile.h>
#include <qstringlist.h>

BookInfo::BookInfo()
{
    m_partsCount = -1;
    m_firstID = -1;
    m_lastID = -1;
    m_bookID = -1;
    m_hasInfo = false;
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

QString BookInfo::toString()
{
    QString text;

    text.append(QString("%1-%2;").arg(m_firstID).arg(m_lastID));

    for(int i=0; i<m_partsCount; i++){
        text.append(QString("%1-%2;").arg(firstPage(i+1)).arg(lastPage(i+1)));
    }

    return text;
}

void BookInfo::fromString(QString info)
{
    if(info.isEmpty())
        return;

    m_partsCount = info.count(';')-1;
    QStringList partInfoList;
    bool idsProccessed = false;
    int part=1;

    foreach (QString partInfo, info.split(';', QString::SkipEmptyParts)) {
        if(!idsProccessed) {
            partInfoList = partInfo.split('-', QString::SkipEmptyParts);
            setFirstID(partInfoList.first().toInt());
            setLastID(partInfoList.last().toInt());
            idsProccessed = true;
        } else {
            partInfoList = partInfo.split('-', QString::SkipEmptyParts);
            setFirstPage(partInfoList.first().toInt(), part);
            setLastPage(partInfoList.last().toInt(), part);
            part++;
        }
    }

    m_hasInfo = true;
}

bool BookInfo::haveInfo()
{
    return m_hasInfo;
}
