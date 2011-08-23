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

    /**
      Open the index database
      @throw BookException
      */
    void open();

    /**
      Get the books list model that contians categories and books
      */
    BooksListModel *booksListModel();

    /**
      Get categories list
      */
    BooksListModel *catsListModel();

    /**
      Load the books list model asynchronously
      */
    void loadBooksListModel();

    /**
      Get information about the specified Book
      @param bookID the book id
      @return BookInfo that contains information about the specified Book
      */
    BookInfo *getBookInfo(int bookID);
    BookInfo *getQuranBook();
    LibraryInfo *connectionInfo();
    QList<QPair<int, QString> > getTafassirList();
    int catIdFromName(const QString &cat);
    int addBook(ImportModelNode *book);
    void updateBookMeta(BookInfo *info, bool newBook);
    void setConnectionInfo(LibraryInfo *info);
    void getShoroohPages(BookInfo *info);

    /**
      Change categorie title
      @param catID categorie id
      @param title the new title
      */
    void updateCatTitle(int catID, QString title);

    /**
      Change a categorie parent
      @param catID categorie id
      @param parentID the new parent id
      */
    void updateCatParent(int catID, int parentID);

    /**
      Chane categorie order in the books list model
      @param catID categorie id
      @param catOrder Categorie order
      */
    void updateCatOrder(int catID, int catOrder);

    /**
      Create a new categorie
      @param title Title of the new categorie
      @return the new categorie id
      */
    int addNewCat(const QString &title);

    /**
      Remove a categorie
      @param catID categorie id
      */
    void removeCat(int catID);

    void makeCatPlace(int parentID, int catOrder);

    /**
      Get categorie books count
      @param catID categorie id
      @return books count
      */
    int booksCount(int catID);

    /**
      Change books categorie
      @param fromCat The source categorie
      @param toCat The target categorie
      @return true on success
      */
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
