#ifndef LIBRARYBOOKMANAGER_H
#define LIBRARYBOOKMANAGER_H

#include "databasemanager.h"
#include "librarybook.h"

#include <qhash.h>
#include <qset.h>

class LibraryBook;
class QStandardItemModel;
class AuthorsManager;

class LibraryBookManager : public DatabaseManager
{
    Q_OBJECT

public:
    LibraryBookManager(QObject *parent = 0);
    ~LibraryBookManager();

    static LibraryBookManager *instance();

    void loadModels();
    void clear();

    QStandardItemModel *getModel();
    LibraryBook *getLibraryBook(int bookID);
    LibraryBook *getQuranBook();

    int addBook(LibraryBook *book);

    void beginUpdate();
    void endUpdate();
    bool updateBook(LibraryBook *book);
    bool removeBook(int bookID);

    QList<int> getNonIndexedBooks();
    void setBookIndexStat(int bookID, LibraryBook::IndexFlags indexFlag);

protected:
    int getNewBookID();
    void loadLibraryBooks();

protected:
    AuthorsManager *m_authorsManager;
    QHash<int, LibraryBook*> m_books;
    QSet<LibraryBook*> m_usedBooks;
    LibraryBook* m_quranBook;
};

#endif // LIBRARYBOOKMANAGER_H
