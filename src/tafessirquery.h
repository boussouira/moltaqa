#ifndef TAFESSIRQUERY_H
#define TAFESSIRQUERY_H

#include <qsqlquery.h>

class LibraryBook;

class TafessirQuery : public QSqlQuery
{
public:
    TafessirQuery(QSqlDatabase db, LibraryBook *bookInfo);

    void nextPage(int id);
    void prevPage(int id);
    void page(int page, int part=1);

    int getAyatCount(int sora, int aya);
    int getPageID(int sora, int aya);

    int getHaddithPage(int hadditNum);

protected:
    LibraryBook *m_bookInfo;
};

#endif // TAFESSIRQUERY_H
