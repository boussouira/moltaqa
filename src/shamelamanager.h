#ifndef SHAMELAMANAGER_H
#define SHAMELAMANAGER_H

#include "shamelainfo.h"
#include "libraryinfo.h"
#include "shamelaimportinfo.h"
#include "shamelamapper.h"

#include <qthread.h>
#include <qmutex.h>
#include <qsqldatabase.h>
#include <qstringlist.h>
#include <qhash.h>
#include <qdebug.h>
#include <qmutex.h>

class ShamelaManager
{
public:
    ShamelaManager(ShamelaInfo *info);
    ~ShamelaManager();
    ShamelaMapper *mapper();
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

    AuthorInfo *getAuthorInfo(int id);

    void selectCats();
    void selectAuthors();
    void selectBooks();

    CategorieInfo *nextCat();
    AuthorInfo *nextAuthor();
    ShamelaBookInfo *nextBook();
    ShamelaBookInfo *nextFiltredBook();

    int getBookShareeh(int shamelaID);
    int getBookMateen(int shamelaID);
    QList<ShamelaShareehInfo*> getShareehInfo(int mateen, int shareeh);

protected:
    LibraryInfo *m_library;
    ShamelaInfo *m_info;
    ShamelaMapper *m_mapper;
    QSqlDatabase m_indexDB;
    QSqlDatabase m_shamelaDB;
    QSqlDatabase m_shamelaSpecialDB;
    QSqlQuery *m_indexQuery;
    QSqlQuery *m_shamelaQuery;
    QSqlQuery *m_shamelaSpecialQuery;
    bool m_haveBookFilter;
    QList<int> m_accepted;
    QList<int> m_rejected;
    QList<int> m_addedShorooh;
    QMutex m_mutex;
};

#endif // SHAMELAMANAGER_H
