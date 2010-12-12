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

