#include "sqlitesimplequery.h"
#include "bookinfo.h"

#include <qvariant.h>

SqliteSimpleQuery::SqliteSimpleQuery()
{
}

SqliteSimpleQuery::SqliteSimpleQuery(QSqlDatabase db):
        SimpleQuery(db)
{
}

SqliteSimpleQuery::SqliteSimpleQuery(QSqlDatabase db, BookInfo *bookInfo):
        SimpleQuery(db, bookInfo)
{
}

void SqliteSimpleQuery::nextPage(int id)
{
    exec(QString("SELECT id, nass, part, page from %1 "
                         "WHERE id >= %2 ORDER BY id ASC LIMIT 1")
                 .arg(m_bookInfo->bookTable())
                 .arg(id));
}

void SqliteSimpleQuery::prevPage(int id)
{
    exec(QString("SELECT id, nass, part, page from %1 "
                         "WHERE id <= %2 ORDER BY id DESC LIMIT 1")
                 .arg(m_bookInfo->bookTable())
                 .arg(id));
}

void SqliteSimpleQuery::page(int page, int part)
{
    exec(QString("SELECT id FROM %1 "
                         "WHERE page >= %2 AND part = %3 "
                         "ORDER BY id ASC LIMIT 1")
                 .arg(m_bookInfo->bookTable())
                 .arg(page)
                 .arg(part));
}

void SqliteSimpleQuery::index()
{
    exec(QString("SELECT id, tit, lvl, sub FROM %1 ORDER BY id")
                 .arg(m_bookInfo->titleTable()));
}

void SqliteSimpleQuery::partInfo(int part)
{
    Q_UNUSED(part)
}

void SqliteSimpleQuery::parts()
{

}
