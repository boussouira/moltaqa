#ifndef BOOKSINDEXDB_H
#define BOOKSINDEXDB_H

#include "bookinfo.h"
#include <qsettings.h>
#include <qsqldatabase.h>

class QAbstractItemModel;
class BooksListModel;
class BooksListNode;

class BooksIndexDB
{
public:
    BooksIndexDB();
    QAbstractItemModel *getListModel(bool books=true);
    int getCatIdFromName(const QString &cat);

protected:
    void booksCat(BooksListNode *parentNode, int catID);
    void childCats(BooksListNode *parentNode, int pID, bool books=true);

protected:
    QSqlDatabase m_booksListDB;
    QString m_appDir;
    QString m_booksFolder;
    QString m_indexDBName;
};

#endif // BOOKSINDEXDB_H
