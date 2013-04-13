#ifndef LIBRARYBOOK_H
#define LIBRARYBOOK_H

#include "bookpage.h"

#include <qdebug.h>
#include <qhash.h>
#include <qsharedpointer.h>

class QDomElement;
class XmlDomHelper;

class BookShorooh
{
public:
    BookShorooh() : pageID(0) {}

    QString bookUUID;
    QString title;
    int pageID;
};

class LibraryBookMeta
{
public:
    LibraryBookMeta();

    typedef QSharedPointer<LibraryBookMeta> Ptr;

    int id;
    uint createDate;
    uint importDate;
    uint updateDate;
    int openCount;
    int updateCount;
    int resultOpenCount;
    QString fileChecksum;

    QString createDateStr();
    QString importDateStr();
    QString updateDateStr();

protected:
    QString formatDate(uint timestamp);
};

class LibraryBook
{
public:
    LibraryBook();
    ~LibraryBook();

    typedef QSharedPointer<LibraryBook> Ptr;

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

    void fromDomElement(QDomElement &bookElement);
    void toDomElement(XmlDomHelper &domHeleper, QDomElement &bookElement);

public:
    LibraryBook::Type type;
    QString uuid;
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

QDebug operator<<(QDebug dbg, LibraryBook::Ptr &info);

#endif // LIBRARYBOOK_H
