#include "quranquery.h"

QuranQuery::QuranQuery()
{
}

QuranQuery::QuranQuery(QSqlDatabase db) : QSqlQuery(db)
{
}

QuranQuery::QuranQuery(QSqlDatabase db, BookInfo *bookInfo) : QSqlQuery(db), m_bookInfo(bookInfo)
{
}

