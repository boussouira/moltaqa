#ifndef INDEXDB_H
#define INDEXDB_H

#include <qsqldatabase.h>
#include <qcoreapplication.h>

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
    virtual ~IndexDB();
    virtual void open()=0;
    virtual QAbstractItemModel *booksList(bool onlyCats=false)=0;
    virtual int catIdFromName(const QString &cat)=0;
    virtual BookInfo *getBookInfo(int bookID)=0;
    virtual int addBook(ImportModelNode *book)=0;
    void setConnectionInfo(LibraryInfo *info);
    LibraryInfo *connectionInfo();

protected:
    LibraryInfo *m_libraryInfo;
    QSqlDatabase m_indexDB;
};

#endif // INDEXDB_H
