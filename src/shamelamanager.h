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
#include <qstandarditemmodel.h>

class ShamelaManager
{
public:
    ShamelaManager(ShamelaInfo *info);
    ~ShamelaManager();
    ShamelaMapper *mapper();
    void setFilterBooks(bool filter);
    void setAcceptedBooks(QList<int> accepted);

    void openIndexDB();
    void openShamelaDB();
    void openShamelaSpecialDB();
    void close();

    QStandardItemModel *getBooksListModel();

    int getBooksCount();
    int getAuthorsCount();
    int getCatCount();

    ShamelaAuthorInfo *getAuthorInfo(int id);

    void selectCats();
    void selectAuthors();
    void selectBooks();

    ShamelaCategorieInfo *nextCat();
    ShamelaAuthorInfo *nextAuthor();
    ShamelaBookInfo *nextBook();
    ShamelaBookInfo *nextFiltredBook();

    int getBookShareeh(int shamelaID);
    int getBookMateen(int shamelaID);
    QList<ShamelaShareehInfo*> getShareehInfo(int mateen, int shareeh);

    static QString mdbTable(QString table);

protected:
    void booksCat(QStandardItem *parentNode, int catID);

protected:
    LibraryInfo *m_library;
    ShamelaInfo *m_info;
    ShamelaMapper *m_mapper;
    QSqlDatabase m_libraryManager;
    QSqlDatabase m_shamelaDB;
    QSqlDatabase m_shamelaSpecialDB;
    QSqlQuery *m_indexQuery;
    QSqlQuery *m_shamelaQuery;
    QSqlQuery *m_shamelaSpecialQuery;
    bool m_haveBookFilter;
    QList<int> m_accepted;
    QList<int> m_addedShorooh;
    QMutex m_mutex;
    QString m_tempShamelaDB;
    QString m_tempshamelaSpecialDB;
};

#endif // SHAMELAMANAGER_H
