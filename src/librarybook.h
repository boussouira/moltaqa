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

    enum BookFlags {
        NoBookFlags = 0,
        PrintedPageNumber = 2,
        MakhetotPageNumer = 4,

        MoqabalMoteboa = 8,
        MoqabalMakhetot = 16,
        MoqabalPdf = 32,

        LinkedWithShareeh = 64,
        HaveFootNotes = 128,
        Mashekool = 256
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
    QString comment;
    QString info;
    int id;
    int authorID;
    BookFlags bookFlags;
    IndexFlags indexFlags;
};

typedef QSharedPointer<LibraryBook> LibraryBookPtr;

QDebug operator<<(QDebug dbg, LibraryBookPtr &info);

#endif // LIBRARYBOOK_H
