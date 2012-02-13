#ifndef LIBRARYBOOKMANAGER_H
#define LIBRARYBOOKMANAGER_H

#include "listmanager.h"
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
    
    void loadModels();
    void clear();

    QStandardItemModel *getModel();
    LibraryBook *getLibraryBook(int bookID);
    LibraryBook *getQuranBook();

    int getNewBookID();
    void addBook(LibraryBook *book);

    void beginUpdate();
    void endUpdate();
    void updateBook(LibraryBook *book);

protected:
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
