#ifndef INDEXDB_H
#define INDEXDB_H

#include <qsqldatabase.h>
#include <qcoreapplication.h>
#include <qhash.h>

class LibraryInfo;
class BookInfo;
class BooksListModel;
class BooksListNode;
class ImportModelNode;
class QAbstractItemModel;

class IndexDB
{
    Q_DECLARE_TR_FUNCTIONS(IndexDB);

public:
    IndexDB();
    IndexDB(LibraryInfo *info);
    ~IndexDB();
    void open();
    QAbstractItemModel *booksList(bool onlyCats=false);
    BookInfo *getBookInfo(int bookID);
    BookInfo *getQuranBook();
    QList<QPair<int, QString> > getTafassirList();
    int catIdFromName(const QString &cat);
    int addBook(ImportModelNode *book);
    void updateBookMeta(BookInfo *info, bool newBook);
    void setConnectionInfo(LibraryInfo *info);
    LibraryInfo *connectionInfo();

protected:
    void booksCat(BooksListNode *parentNode, int catID);
    void childCats(BooksListNode *parentNode, int pID, bool onlyCats=false);

protected:
    LibraryInfo *m_libraryInfo;
    QSqlDatabase m_indexDB;
};

#endif // INDEXDB_H
