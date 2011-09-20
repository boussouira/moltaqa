#ifndef LIBRARYMANAGER_H
#define LIBRARYMANAGER_H

#include <qobject.h>
#include <qsqldatabase.h>
#include <qcoreapplication.h>
#include <qhash.h>
#include <QtConcurrentRun>
#include <qfuture.h>
#include <qfuturewatcher.h>
#include <qstandarditemmodel.h>

class LibraryInfo;
class BookInfo;
class BooksListModel;
class BooksListNode;
class ImportModelNode;

class LibraryManager : public QObject
{
    Q_OBJECT
public:
    LibraryManager(LibraryInfo *info, QObject *parent=0);
    ~LibraryManager();

    /**
      Open the index database
      @throw BookException
      */
    void open();

    void transaction();
    bool commit();
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
    BookInfo *getBookInfo(int bookID, bool allInfo = false);
    BookInfo *getQuranBook();
    LibraryInfo *connectionInfo();
    QList<QPair<int, QString> > getTafassirList();
    QHash<int, QString> getCategoriesList();
    QPair<int, QString> findCategorie(const QString &cat);
    QPair<int, QString> findAuthor(const QString &name);
    int addBook(ImportModelNode *book);
    void updateBookMeta(BookInfo *info, bool newBook);
    void setConnectionInfo(LibraryInfo *info);
    bool hasShareeh(int bookID);
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
      Get categorie count
      */

    int categoriesCount();
    /**
      Change books categorie
      @param fromCat The source categorie
      @param toCat The target categorie
      @return true on success
      */
    bool moveCatBooks(int fromCat, int toCat);

    QStandardItemModel *getAuthorsListModel();
    void updateBookInfo(BookInfo *info);

protected:
    void booksCat(BooksListNode *parentNode, int catID);
    void childCats(BooksListNode *parentNode, int pID, bool onlyCats=false);
    void loadModel();

protected slots:
    void booksListModelLoaded();

signals:
    void booksListModelLoaded(BooksListModel *model);

protected:
    BooksListModel *m_model;
    LibraryInfo *m_libraryInfo;
    QSqlDatabase m_libraryManager;
    QFuture<void> m_future;
    QFutureWatcher<void> m_watcher;
    QString m_connName;
};

#endif // LIBRARYMANAGER_H
