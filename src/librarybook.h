#ifndef LIBRARYBOOK_H
#define LIBRARYBOOK_H

#include "bookpage.h"
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

class LibraryBook
{
public:
    LibraryBook();
    ~LibraryBook();

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

    LibraryBook *clone();

public:
    LibraryBook::Type bookType;
    QString bookPath;
    QString fileName;
    QString bookDisplayName;
    QString bookOtherNames;
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
    int flags;
    bool hasShareeh;

protected:
    QHash<int, int> m_firstPages;
    QHash<int, int> m_lastPages;
    bool m_hasInfo;
};

QDebug operator<<(QDebug, LibraryBook *);

#endif // LIBRARYBOOK_H
