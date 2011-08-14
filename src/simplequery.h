#ifndef SIMPLEQUERY_H
#define SIMPLEQUERY_H

#include <qsqlquery.h>

class BookInfo;

class SimpleQuery : public QSqlQuery
{
public:
    SimpleQuery();
    SimpleQuery(QSqlDatabase db);
    SimpleQuery(QSqlDatabase db, BookInfo *bookInfo);
    void setBookInfo(BookInfo *bookInfo);

    void nextPage(int id);
    void prevPage(int id);
    void page(int page, int part=1);
    void index();
    int getHaddithNumber(int pageID);
    void partInfo(int part);

protected:
    BookInfo *m_bookInfo;
};

#endif // SIMPLEQUERY_H
