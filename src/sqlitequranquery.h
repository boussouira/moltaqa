#ifndef SQLITEQURANQUERY_H
#define SQLITEQURANQUERY_H

#include "quranquery.h"

class SqliteQuranQuery : public QuranQuery
{
public:
    SqliteQuranQuery();
    SqliteQuranQuery(QSqlDatabase db);
    SqliteQuranQuery(QSqlDatabase db, BookInfo *bookInfo);

    void page(int page);
    void index();
    void pageNumber(int aya, int sora);
    void soraAyatCount(int sora);
    void firstSoraAndAya(int page);
};

#endif // SQLITEQURANQUERY_H
