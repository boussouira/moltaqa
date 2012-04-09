#ifndef LIBRARYBOOKMANAGER_H
#define LIBRARYBOOKMANAGER_H

#include "databasemanager.h"
#include "librarybook.h"

#include <qhash.h>
#include <qset.h>

class QStandardItemModel;
class AuthorsManager;

class LibraryBookManager : public DatabaseManager
{
    Q_OBJECT

public:
    LibraryBookManager(QObject *parent = 0);
    ~LibraryBookManager();

    void loadModels();
    void clear();

    QStandardItemModel *getModel();
    LibraryBookPtr getLibraryBook(int bookID);
    LibraryBookPtr getQuranBook();

    int addBook(LibraryBookPtr book);

    void beginUpdate();
    void endUpdate();
    bool updateBook(LibraryBookPtr book);
    bool removeBook(int bookID);

    QList<int> getBooksWithIndexStat(LibraryBook::IndexFlags indexFlag);
    void setBookIndexStat(int bookID, LibraryBook::IndexFlags indexFlag);

    QList<LibraryBookPtr> getAuthorBooks(int authorID);

protected:
    int getNewBookID();
    void loadLibraryBooks();

protected:
    AuthorsManager *m_authorsManager;
    QHash<int, LibraryBookPtr> m_books;
    LibraryBookPtr m_quranBook;
};

#endif // LIBRARYBOOKMANAGER_H
