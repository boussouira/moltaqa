#ifndef TAFESSIRDBHANDLER_H
#define TAFESSIRDBHANDLER_H

#include "abstractdbhandler.h"

class TafessirQuery;
class BookIndexNode;
class TafessirTextFormat;

class TafessirDBHandler : public AbstractDBHandler
{
public:
    TafessirDBHandler();
    ~TafessirDBHandler();

    void nextPage();
    void prevPage();

    bool hasNext();
    bool hasPrev();

    QAbstractItemModel *indexModel();
    QAbstractItemModel *topIndexModel();

    void openSora(int sora, int aya);

protected:
    void openID(int pid = -1);
    void openPage(int page, int part =1);
    void getBookInfo();
    void childTitles(BookIndexNode *parentNode, int tid);
    void connected();
    void openQuranBook();
    void readQuranText(int sora, int aya, int count);

protected:
    TafessirTextFormat *m_formatter;
    QSqlDatabase m_quranDB;
    QSqlQuery *m_quranQuery;
    BookInfo *m_quranInfo;
    TafessirQuery *m_tafessirQuery;
};

#endif // TAFESSIRDBHANDLER_H
