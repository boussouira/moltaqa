#include "bookinfo.h"
#include <qfile.h>
#include <qstringlist.h>

BookInfo::BookInfo()
{
    partsCount = -1;
    firstID = -1;
    lastID = -1;
    bookID = -1;
    m_hasInfo = false;
}

void BookInfo::setFirstPage(int count, int part)
{
    if(part <= partsCount)
        m_firstPages.insert(part, count);
    else
        qWarning("The given part(%d) is out of range(%d)", part, partsCount);
}

int BookInfo::firstPage(int part)
{
    if(part <= partsCount)
        return m_firstPages.value(part);
    else {
        qWarning("The given part(%d) is out of range(%d)", part, partsCount);
        return 0;
    }
}

void BookInfo::setLastPage(int count, int part)
{
    if(part <= partsCount)
        m_lastPages.insert(part, count);
    else
        qWarning("The given part(%d) is out of range(%d)", part, partsCount);
}

int BookInfo::lastPage(int part)
{
    if(part <= partsCount)
        return m_lastPages.value(part);
    else {
        qWarning("The given part(%d) is out of range(%d)", part, partsCount);
        return 0;
    }
}

bool BookInfo::exists()
{
    if(!bookPath.isEmpty()){
        return QFile::exists(bookPath);
    } else {
        qWarning("Call to BookInfo::exists() before BookInfo::setBookPath()");
        return false;
    }
}

QString BookInfo::toString()
{
    QString text;

    text.append(QString("%1-%2;").arg(firstID).arg(lastID));
    text.append(QString("%1:%2-%3").arg(partsCount).arg(firstPage()).arg(lastPage()));

    return text;
}

void BookInfo::fromString(QString info)
{
    if(info.isEmpty())
        return;

    partsCount = info.count(';');
    QStringList partInfoList;
    bool idsProccessed = false;
    int part=1;

    foreach (QString partInfo, info.split(';', QString::SkipEmptyParts)) {
        if(!idsProccessed) {
            partInfoList = partInfo.split('-', QString::SkipEmptyParts);
            firstID = partInfoList.first().toInt();
            lastID = partInfoList.last().toInt();
            idsProccessed = true;
        } else {
            partInfoList = partInfo.split(':', QString::SkipEmptyParts);
            partsCount = partInfoList.first().toInt();
            partInfoList = partInfoList.last().split('-', QString::SkipEmptyParts);
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
