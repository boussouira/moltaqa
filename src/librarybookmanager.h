#ifndef LIBRARYBOOKMANAGER_H
#define LIBRARYBOOKMANAGER_H

#include "listmanager.h"
#include "librarybook.h"

#include <qhash.h>
#include <qset.h>

class LibraryBook;
class QStandardItemModel;
class AuthorsManager;

class LibraryBookManager : public ListManager
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

    void addBook(LibraryBook *book);

    void beginUpdate();
    void endUpdate();
    void updateBook(LibraryBook *book);

    QList<int> getNonIndexedBooks();
    void setBookIndexStat(int bookID, LibraryBook::IndexFlags indexFlag);

protected:
    int getNewBookID();
    void readBook(QDomElement &e);
    void loadLibraryBooks();

protected:
    AuthorsManager *m_authorsManager;
    QHash<int, LibraryBook*> m_books;
    QHash<int, QDomElement> m_bookElementHash;
    QSet<LibraryBook*> m_usedBooks;
    LibraryBook* m_quranBook;
};

#endif // LIBRARYBOOKMANAGER_H
