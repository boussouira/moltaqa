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
        AutoPageNumber = 1,
        PrintedPageNumber = 2,
        MakhetotPageNumer = 4,

        NotMoqabal = 8,
        MoqabalMoteboa = 16,
        MoqabalMakhetot = 32,
        MoqabalPdf = 64,

        LinkedWithShareeh = 128,
        HaveFootNotes = 256,
        Mashekool = 512
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
