#ifndef RICHTAFESSIRREADER_H
#define RICHTAFESSIRREADER_H

#include "richbookreader.h"

class TafessirQuery;
class BookIndexNode;
class TafessirTextFormat;

class RichTafessirReader : public RichBookReader
{
public:
    RichTafessirReader(QObject *parent=0);
    ~RichTafessirReader();

    void goToPage(int pid = -1);
    void goToPage(int page, int part);
    void goToSora(int sora, int aya);
    void goToHaddit(int hadditNum);

    QAbstractItemModel *indexModel();
    QAbstractItemModel *topIndexModel();

protected:
    void childTitles(BookIndexNode *parentNode, int tid);
    void connected();
    void openQuranBook();
    void readQuranText(int sora, int aya, int count);

protected:
    TafessirTextFormat *m_formatter;
    QSqlDatabase m_quranDB;
    QSqlQuery *m_quranQuery;
    LibraryBook *m_quranInfo;
    TafessirQuery *m_tafessirQuery;
};

#endif // RICHTAFESSIRREADER_H