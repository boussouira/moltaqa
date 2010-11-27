#ifndef ABSTRACTDBHANDLER_H
#define ABSTRACTDBHANDLER_H

#include <qobject.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qcoreapplication.h>

class BookInfo;
class BookIndexModel;
class QAbstractItemModel;
class QSqlQuery;

class AbstractDBHandler
{
    Q_DECLARE_TR_FUNCTIONS(AbstractDBHandler);

public:
    AbstractDBHandler();
    virtual ~AbstractDBHandler();
    void openBookDB(QString pBookDBPath=QString());
    BookInfo *bookInfo() { return m_bookInfo; }
    void setBookInfo(BookInfo *bi);

public:
    virtual QString openPage(int page, int part=1) = 0;
    virtual QString openID(int id = -1) = 0;
    virtual QString openIndexID(int pid = -1);
    virtual QString nextPage() = 0;
    virtual QString prevPage() = 0;

    virtual QString nextUnit();
    virtual QString prevUnit();

    virtual bool hasNext() = 0;
    virtual bool hasPrev() = 0;

    virtual QAbstractItemModel *indexModel() = 0;
    virtual void getBookInfo() = 0;

protected:
    BookInfo *m_bookInfo;
    BookIndexModel *m_indexModel;
    QSqlDatabase m_bookDB;
    QSqlQuery m_bookQuery;
    QString m_bookDBPath;
    QString m_connectionName;
};

#endif // ABSTRACTDBHANDLER_H
