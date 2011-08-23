#ifndef INDEXDB_H
#define INDEXDB_H

#include <qobject.h>
#include <qsqldatabase.h>
#include <qcoreapplication.h>
#include <qhash.h>
#include <QtConcurrentRun>
#include <qfuture.h>

class LibraryInfo;
class BookInfo;
class BooksListModel;
class BooksListNode;
class ImportModelNode;

class IndexDB
{
    Q_DECLARE_TR_FUNCTIONS(IndexDB);

public:
    IndexDB(LibraryInfo *info);
    ~IndexDB();
    void open();
    BooksListModel *booksListModel();
    BooksListModel *catsListModel();
    void loadBooksListModel();
    BookInfo *getBookInfo(int bookID);
    BookInfo *getQuranBook();
    LibraryInfo *connectionInfo();
    QList<QPair<int, QString> > getTafassirList();
    int catIdFromName(const QString &cat);
    int addBook(ImportModelNode *book);
    void updateBookMeta(BookInfo *info, bool newBook);
    void setConnectionInfo(LibraryInfo *info);
    void getShoroohPages(BookInfo *info);

    void updateCatTitle(int catID, QString title);
    void updateCatParent(int catID, int parentID);
    void updateCatOrder(int catID, int catOrder);
    int addNewCat(const QString &title);
    void removeCat(int catID);
    void makeCatPlace(int parentID, int catOrder);
    int booksCount(int catID);
    bool moveCatBooks(int fromCat, int toCat);

protected:
    void booksCat(BooksListNode *parentNode, int catID);
    void childCats(BooksListNode *parentNode, int pID, bool onlyCats=false);
    void loadModel();

protected:
    BooksListModel *m_model;
    LibraryInfo *m_libraryInfo;
    QSqlDatabase m_indexDB;
    QFuture<void> m_future;
};

#endif // INDEXDB_H
