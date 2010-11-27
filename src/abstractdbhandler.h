#ifndef ABSTRACTDBHANDLER_H
#define ABSTRACTDBHANDLER_H

#include <qobject.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qcoreapplication.h>

class BookInfo;
class BookIndexModel;
class TextFormatter;
class QAbstractItemModel;
class QSqlQuery;

class AbstractDBHandler
{
    Q_DECLARE_TR_FUNCTIONS(AbstractDBHandler);

public:
    AbstractDBHandler();
    virtual ~AbstractDBHandler();
    void openBookDB(QString pBookDBPath=QString());
    void setBookInfo(BookInfo *bi);
    BookInfo *bookInfo() { return m_bookInfo; }
    TextFormatter *textFormatter() { return m_textFormat; }

public:
    virtual void openPage(int page, int part=1) = 0;
    virtual void openID(int id = -1) = 0;
    virtual void openIndexID(int pid = -1);
    virtual void nextPage() = 0;
    virtual void prevPage() = 0;

    virtual void nextUnit();
    virtual void prevUnit();

    virtual bool hasNext() = 0;
    virtual bool hasPrev() = 0;

    virtual QAbstractItemModel *indexModel() = 0;
    virtual void getBookInfo() = 0;

protected:
    BookInfo *m_bookInfo;
    BookIndexModel *m_indexModel;
    TextFormatter *m_textFormat;
    QSqlDatabase m_bookDB;
    QSqlQuery m_bookQuery;
    QString m_bookDBPath;
    QString m_connectionName;
};

#endif // ABSTRACTDBHANDLER_H
