#ifndef QURANQUERY_H
#define QURANQUERY_H

#include <qsqlquery.h>

class BookInfo;

class QuranQuery : public QSqlQuery
{
public:
    QuranQuery();
    QuranQuery(QSqlDatabase db);
    QuranQuery(QSqlDatabase db, BookInfo *bookInfo);
    void setBookInfo(BookInfo *bookInfo);

    virtual void page(int page)=0;
    virtual void index()=0;
    virtual void pageNumber(int aya, int sora)=0;
    virtual void soraAyatCount(int sora)=0;
    virtual void firstSoraAndAya(int page)=0;

protected:
    BookInfo *m_bookInfo;
};

#endif // QURANQUERY_H
