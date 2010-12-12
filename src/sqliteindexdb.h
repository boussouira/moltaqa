#ifndef SQLITEINDEXDB_H
#define SQLITEINDEXDB_H

#include <indexdb.h>

class SqliteIndexDB : public IndexDB
{
public:
    SqliteIndexDB();
    SqliteIndexDB(ConnectionInfo *info);
    ~SqliteIndexDB();
    void open();
    QAbstractItemModel *booksList(bool onlyCats=false);
    int catIdFromName(const QString &cat);
    BookInfo *getBookInfo(int bookID);
    int addBook(ImportModelNode *book);

protected:
    void booksCat(BooksListNode *parentNode, int catID);
    void childCats(BooksListNode *parentNode, int pID, bool onlyCats=false);
};

#endif // SQLITEINDEXDB_H
