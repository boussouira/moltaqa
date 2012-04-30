#ifndef LIBRARYBOOKMANAGER_H
#define LIBRARYBOOKMANAGER_H

#include "databasemanager.h"
#include "librarybook.h"

#include <qhash.h>
#include <qset.h>
#include <qsharedpointer.h>

class QStandardItemModel;
class AuthorsManager;
class LibraryInfo;

typedef QSharedPointer<QStandardItemModel> StandardItemModelPtr;

class LibraryBookManager : public DatabaseManager
{
    Q_OBJECT

public:
    LibraryBookManager(QObject *parent = 0);
    ~LibraryBookManager();

    void loadModels();
    void clear();

    StandardItemModelPtr getModel();
    StandardItemModelPtr getLastOpendModel();
    StandardItemModelPtr getBookHistoryModel(int bookID);

    LibraryBookPtr getLibraryBook(int bookID);
    LibraryBookPtr getQuranBook();

    int addBook(LibraryBookPtr book);

    bool updateBook(LibraryBookPtr book);
    bool removeBook(int bookID);

    QList<int> getBooksWithIndexStat(LibraryBook::IndexFlags indexFlag);
    void setBookIndexStat(int bookID, LibraryBook::IndexFlags indexFlag);

    QList<LibraryBookPtr> getAuthorBooks(int authorID);

    void addBookHistory(int bookID, int pageID);
    /// Get the last readed for the given book
    int bookLastPageID(int bookID);

protected:
    int getNewBookID();

protected:
    LibraryInfo *m_libraryInfo;
    AuthorsManager *m_authorsManager;
    QHash<int, LibraryBookPtr> m_books;
    LibraryBookPtr m_quranBook;
};

#endif // LIBRARYBOOKMANAGER_H
