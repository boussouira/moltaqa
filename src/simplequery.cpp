#include "simplequery.h"
#include "bookinfo.h"

SimpleQuery::SimpleQuery()
{
}

SimpleQuery::SimpleQuery(QSqlDatabase db) : QSqlQuery(db)
{
}

SimpleQuery::SimpleQuery(QSqlDatabase db, BookInfo *bookInfo) : QSqlQuery(db), m_bookInfo(bookInfo)
{
}

void SimpleQuery::nextPage(int id)
{
    exec(QString("SELECT id, pageText, partNum, pageNum from %1 "
                         "WHERE id >= %2 ORDER BY id ASC LIMIT 1")
                 .arg(m_bookInfo->bookTable())
                 .arg(id));
}

void SimpleQuery::prevPage(int id)
{
    exec(QString("SELECT id, pageText, partNum, pageNum from %1 "
                         "WHERE id <= %2 ORDER BY id DESC LIMIT 1")
                 .arg(m_bookInfo->bookTable())
                 .arg(id));
}

void SimpleQuery::page(int page, int part)
{
    exec(QString("SELECT id FROM %1 "
                         "WHERE pageNum >= %2 AND partNum = %3 "
                         "ORDER BY id ASC LIMIT 1")
                 .arg(m_bookInfo->bookTable())
                 .arg(page)
                 .arg(part));
}

void SimpleQuery::index()
{
    exec(QString("SELECT id, title, parentID FROM %1 ORDER BY id")
                 .arg(m_bookInfo->titleTable()));
}

void SimpleQuery::partInfo(int part)
{
    Q_UNUSED(part)
}

void SimpleQuery::parts()
{

}
