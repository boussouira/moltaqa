#ifndef SHAMELAMANAGER_H
#define SHAMELAMANAGER_H

#include "shamelainfo.h"
#include "libraryinfo.h"
#include "shamelaimportinfo.h"

#include <qthread.h>
#include <qmutex.h>
#include <qsqldatabase.h>
#include <qstringlist.h>
#include <qhash.h>
#include <qdebug.h>

class ShamelaManager
{
public:
    ShamelaManager(ShamelaInfo *info);
    ~ShamelaManager();
    void setFilterBooks(bool filter);
    void setAcceptedBooks(QList<int> accepted);
    void setRejectedBooks(QList<int> rejected);

    void openIndexDB();
    void openShamelaDB();
    void openShamelaSpecialDB();
    void close();

    int getBooksCount();
    int getAuthorsCount();
    int getCatCount();

    void selectCats();
    void selectAuthors();
    void selectBooks();

    CategorieInfo *nextCat();
    AuthorInfo *nextAuthor();
    ShamelaBookInfo *nextBook();
    ShamelaBookInfo *nextFiltredBook();

protected:
    LibraryInfo *m_library;
    ShamelaInfo *m_info;
    QSqlDatabase m_indexDB;
    QSqlDatabase m_shamelaDB;
    QSqlDatabase m_shamelaSpecialDB;
    QSqlQuery *m_indexQuery;
    QSqlQuery *m_shamelaQuery;
    QSqlQuery *m_shamelaSpecialQuery;
    bool m_haveBookFilter;
    QList<int> m_accepted;
    QList<int> m_rejected;
};

#endif // SHAMELAMANAGER_H
