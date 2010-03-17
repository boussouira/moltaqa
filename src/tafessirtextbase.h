#ifndef TAFESSIRTEXTBASE_H
#define TAFESSIRTEXTBASE_H

#include <QObject>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringListModel>
#include "qurantextbase.h"
#include "pageinfo.h"
#include "tafessirtextformat.h"

class TafessirTextBase : public QuranTextBase, public TafessirTextFormat
{
public:
    TafessirTextBase();
    QString getTafessirPage(PageInfo *pPageInfo);

protected:
    QSqlDatabase m_tafessirDB;
    QSqlQuery *m_tafessirQuery;
    QString m_tafessirDBPath;
};

#endif // TAFESSIRTEXTBASE_H
