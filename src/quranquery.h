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

    void page(int page);
    void index();
    void pageNumber(int aya, int sora);
    void soraAyatCount(int sora);
    void firstSoraAndAya(int page);

protected:
    BookInfo *m_bookInfo;
};

#endif // QURANQUERY_H
