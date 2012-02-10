#ifndef LIBRARYBOOKMANAGER_H
#define LIBRARYBOOKMANAGER_H

#include "listmanager.h"
#include <qhash.h>

class LibraryBook;
class QStandardItemModel;

class LibraryBookManager : public ListManager
{
    Q_OBJECT

public:
    LibraryBookManager(QObject *parent = 0);
    ~LibraryBookManager();
    
    void loadLibraryBooks();
    void reloadLibraryBooks();
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

protected:
    QHash<int, LibraryBook*> m_books;
    QHash<int, QDomElement> m_bookElementHash;
    LibraryBook* m_quranBook;
};

#endif // LIBRARYBOOKMANAGER_H
