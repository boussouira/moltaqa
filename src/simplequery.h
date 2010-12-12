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

    virtual void nextPage(int id)=0;
    virtual void prevPage(int id)=0;
    virtual void page(int page, int part=1)=0;
    virtual void index()=0;
    virtual void partInfo(int part)=0;
    virtual void parts()=0;

protected:
    BookInfo *m_bookInfo;
};

#endif // SIMPLEQUERY_H
