#ifndef LIBRARYBOOK_H
#define LIBRARYBOOK_H

#include "bookpage.h"
#include <qhash.h>
#include <qsharedpointer.h>
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
        NormalBook
    };

    enum IndexFlags {
        NotIndexed,
        Indexed,
        Update,
        Delete,
        Ignore
    };

    bool isQuran() { return type == QuranBook; }
    bool isNormal() { return type == NormalBook; }
    bool isTafessir() { return type == TafessirBook; }
    bool exists();

    QList<BookShorooh> shorooh;

    LibraryBook *clone();

public:
    LibraryBook::Type type;
    QString path;
    QString fileName;
    QString title;
    QString otherTitles;
    QString authorName;
    QString edition;
    QString publisher;
    QString mohaqeq;
    QString info;
    int id;
    int authorID;
    int bookFlags;
    IndexFlags indexFlags;
};

typedef QSharedPointer<LibraryBook> LibraryBookPtr;

QDebug operator<<(QDebug dbg, LibraryBookPtr &info);

#endif // LIBRARYBOOK_H
