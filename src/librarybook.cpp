#include "librarybook.h"
#include <qfile.h>
#include <qstringlist.h>

LibraryBook::LibraryBook()
{
    partsCount = -1;
    firstID = -1;
    lastID = -1;
    bookID = -1;
    m_hasInfo = false;
}

LibraryBook::~LibraryBook()
{
    qDeleteAll(shorooh);
    shorooh.clear();
}

void LibraryBook::setFirstPage(int count, int part)
{
    if(part <= partsCount)
        m_firstPages.insert(part, count);
    else
        qWarning("The given part(%d) is out of range(%d)", part, partsCount);
}

int LibraryBook::firstPage(int part)
{
    if(part <= partsCount)
        return m_firstPages.value(part);
    else {
        qWarning("The given part(%d) is out of range(%d)", part, partsCount);
        return 0;
    }
}

void LibraryBook::setLastPage(int count, int part)
{
    if(part <= partsCount)
        m_lastPages.insert(part, count);
    else
        qWarning("The given part(%d) is out of range(%d)", part, partsCount);
}

int LibraryBook::lastPage(int part)
{
    if(part <= partsCount)
        return m_lastPages.value(part);
    else {
        qWarning("The given part(%d) is out of range(%d)", part, partsCount);
        return 0;
    }
}

bool LibraryBook::exists()
{
    if(!bookPath.isEmpty()){
        return QFile::exists(bookPath);
    } else {
        qWarning("Call to BookInfo::exists() before BookInfo::setBookPath()");
        return false;
    }
}

QString LibraryBook::toString()
{
    QString text;

    text.append(QString("%1-%2;").arg(firstID).arg(lastID));
    text.append(QString("%1:%2-%3").arg(partsCount).arg(firstPage()).arg(lastPage()));

    return text;
}

void LibraryBook::fromString(QString info)
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

bool LibraryBook::haveInfo()
{
    return m_hasInfo;
}

LibraryBook *LibraryBook::clone()
{
    LibraryBook *book = new LibraryBook();
    book->bookType = bookType;
    book->bookPath = bookPath;
    book->bookDisplayName = bookDisplayName;
    book->bookFullName = bookFullName;
    book->bookOtherNames = bookOtherNames;
    book->textTable = textTable;
    book->indexTable = indexTable;
    book->metaTable = metaTable;
    book->tafessirTable = tafessirTable;
    book->authorName = authorName;
    book->bookEdition = bookEdition;
    book->bookPublisher = bookPublisher;
    book->bookMohaqeq = bookMohaqeq;
    book->bookInfo = bookInfo;
    book->partsCount = partsCount;
    book->firstID = firstID;
    book->lastID = lastID;
    book->bookID = bookID;
    book->authorID = authorID;
    book->hasShareeh = hasShareeh;

    book->m_firstPages = m_firstPages;
    book->m_lastPages = m_lastPages;
    book->m_hasInfo = m_hasInfo;

    return book;
}

QDebug operator<<(QDebug dbg, LibraryBook *info)
{
    dbg.nospace() << "BookInfo(\n\t"
                  << "Type: " << info->bookType << "\n\t"
                  << "Display name: " << info->bookDisplayName << "\n\t"
                  << "Full name: " << info->bookFullName << "\n\t"
                  << "Path: " << info->bookPath << "\n\t"
                  << "Author: " << info->authorName << "\n\t"
                  << "Edition: " << info->bookEdition << "\n\t"
                  << "Publisher: " << info->bookPublisher << "\n\t"
                  << "Mohaqeq: " << info->bookMohaqeq<< "\n"
                  << ")";

    return dbg.space();
}
