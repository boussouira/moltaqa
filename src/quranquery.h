#ifndef QURANQUERY_H
#define QURANQUERY_H

#include <qsqlquery.h>

class LibraryBook;

class QuranQuery : public QSqlQuery
{
public:
    QuranQuery();
    QuranQuery(QSqlDatabase db);
    QuranQuery(QSqlDatabase db, LibraryBook *bookInfo);
    void setBookInfo(LibraryBook *bookInfo);

    void page(int page);
    void index();
    void pageNumber(int aya, int sora);
    void soraAyatCount(int sora);
    void firstSoraAndAya(int page);

protected:
    LibraryBook *m_bookInfo;
};

#endif // QURANQUERY_H
