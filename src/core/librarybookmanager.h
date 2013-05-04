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

    StandardItemModelPtr getModel(bool bookIcon=true);
    StandardItemModelPtr getLastOpendModel();
    StandardItemModelPtr getBookHistoryModel(int bookID);

    LibraryBook::Ptr getLibraryBook(int bookID);
    LibraryBook::Ptr getLibraryBook(QString bookUUID);
    LibraryBook::Ptr getQuranBook();

    LibraryBookMeta::Ptr getLibraryBookMeta(int bookID);

    LibraryBook::Ptr findBook(QString bookName);
    int booksCount();

    int addBook(LibraryBook::Ptr book);

    bool updateBook(LibraryBook::Ptr book, bool updateMeta);
    bool removeBook(int bookID);

    QList<int> getBooksWithIndexStat(LibraryBook::IndexFlags indexFlag);
    void setBookIndexStat(int bookID, LibraryBook::IndexFlags indexFlag);

    QList<LibraryBook::Ptr> getAuthorBooks(int authorID);

    void increaseOpenCount(int bookID);
    void increaseResultOpenCount(int bookID);

    void addBookHistory(int bookID, int pageID);
    /// Get the last readed for the given book
    int bookLastPageID(int bookID);

    bool deleteBookHistory(int bookID=-1);
    bool deleteBookFromLastOpen(int bookID=-1);

protected:
    int getNewBookID();
    QString getNewBookUUID();

signals:
    void bookRemoved(int bookID);

protected:
    LibraryInfo *m_libraryInfo;
    AuthorsManager *m_authorsManager;
    QHash<int, LibraryBook::Ptr> m_books;
    QHash<QString, int> m_uuid;
    LibraryBook::Ptr m_quranBook;
};

#endif // LIBRARYBOOKMANAGER_H
