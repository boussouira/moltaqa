#ifndef SQLITESIMPLEQUERY_H
#define SQLITESIMPLEQUERY_H

#include "simplequery.h"

class SqliteSimpleQuery : public SimpleQuery
{
public:
    SqliteSimpleQuery();
    SqliteSimpleQuery(QSqlDatabase db);
    SqliteSimpleQuery(QSqlDatabase db, BookInfo *bookInfo);

    void nextPage(int id);
    void prevPage(int id);
    void page(int page, int part=1);
    void index();
    void partInfo(int part);
    void parts();
};

#endif // SQLITESIMPLEQUERY_H
