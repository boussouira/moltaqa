#ifndef ABSTRACTDBHANDLER_H
#define ABSTRACTDBHANDLER_H

#include <QObject>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringListModel>
#include "pageinfo.h"
#include "bookindexmodel.h"

class AbstractDBHandler
{
public:
    AbstractDBHandler();
    void openQuranDB(QString pQuranDBPath);

protected:
    virtual QString nextPage() = 0;
    virtual QString prevPage() = 0;

    virtual void nextUnit() = 0;
    virtual void prevUnit() = 0;

    virtual bool canGoNext() = 0;
    virtual bool canGoPrev() = 0;

    virtual QAbstractItemModel *indexModel() = 0;
    virtual QString getFormattedPage() = 0;


protected:
    PageInfo *m_pageInfo;
    BookIndexModel *m_indexModel;
    QSqlDatabase m_bookDB;
    QSqlQuery *m_bookQuery;
    QString m_bookDBPath;
};

#endif // ABSTRACTDBHANDLER_H
