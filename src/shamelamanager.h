#ifndef SHAMELAMANAGER_H
#define SHAMELAMANAGER_H

#include "shamelainfo.h"
#include "shamelaimportinfo.h"
#include "shamelamapper.h"
#include "sqlutils.h"

#include <qthread.h>
#include <qmutex.h>
#include <qsqldatabase.h>
#include <qstringlist.h>
#include <qhash.h>
#include <qdebug.h>
#include <qmutex.h>
#include <qstandarditemmodel.h>

typedef QList<QPair<QString, QString> > ShoortsList;

class ShamelaImportDialog;

class ShamelaManager
{
public:
    ShamelaManager(ShamelaInfo *info);
    ~ShamelaManager();
    ShamelaMapper *mapper();
    void setFilterBooks(bool filter);
    void setAcceptedBooks(QList<int> accepted);

    void openShamelaDB();
    void openShamelaSpecialDB();
    void close();

    QStandardItemModel *getBooksListModel();

    int getBooksCount();
    int getAuthorsCount();
    int getCatCount();

    ShamelaAuthorInfo *getAuthorInfo(int id);
    ShoortsList getBookShoorts(int bookID);

    void selectCats();
    void selectAuthors();
    void selectBooks();

    ShamelaCategorieInfo *nextCat();
    ShamelaAuthorInfo *nextAuthor();
    ShamelaBookInfo *nextBook();
    ShamelaBookInfo *nextFiltredBook();

    void importShorooh(ShamelaImportDialog *dialog);
    QList<int> getBookShorooh(int shamelaID);

    static QString mdbTable(QString table);


protected:
    void booksCat(QStandardItem *parentNode, int catID);

protected:
    DatabaseRemover m_remover;
    ShamelaInfo *m_info;
    ShamelaMapper *m_mapper;
    QSqlDatabase m_shamelaDB;
    QSqlDatabase m_shamelaSpecialDB;
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
