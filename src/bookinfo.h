#ifndef BOOKINFO_H
#define BOOKINFO_H

#include "pageinfo.h"
#include <qhash.h>
#include <qdebug.h>

class BookShorooh
{
public:
    BookShorooh(int bid, int pid, QString bname) : bookID(bid), pageID(pid), bookName(bname){}
    int bookID;
    int pageID;
    QString bookName;
};

class BookInfo
{
public:
    BookInfo();

    enum Type {
        QuranBook = 1,
        TafessirBook,
        NormalBook };

    int firstPage(int part=1);
    int lastPage(int part=1);

    void setPagesCount(int count, int part=1);
    void setFirstPage(int count, int part=1);
    void setLastPage(int count, int part=1);

    bool isQuran() { return bookType == QuranBook; }
    bool isNormal() { return bookType == NormalBook; }
    bool isTafessir() { return bookType == TafessirBook; }
    bool exists();

    QString toString();
    void fromString(QString info);
    bool haveInfo();
    QList<BookShorooh*> shorooh;

public:
    BookInfo::Type bookType;
    QString bookPath;
    QString bookDisplayName;
    QString bookFullName;
    QString bookOtherNames;
    QString textTable;
    QString indexTable;
    QString metaTable;
    QString tafessirTable;
    QString authorName;
    QString bookEdition;
    QString bookPublisher;
    QString bookMohaqeq;
    QString bookInfo;
    int partsCount;
    int firstID;
    int lastID;
    int bookID;
    int authorID;
    bool hasShareeh;
    PageInfo currentPage;

protected:
    QHash<int, int> m_firstPages;
    QHash<int, int> m_lastPages;
    bool m_hasInfo;
};

QDebug operator<<(QDebug, BookInfo *);

#endif // BOOKINFO_H
