#ifndef QURANDBHANDLER_H
#define QURANDBHANDLER_H

#include "abstractdbhandler.h"

class QuranQuery;
class QuranTextFormat;

class QuranDBHandler : public AbstractDBHandler
{
public:
    QuranDBHandler();
    ~QuranDBHandler();

    void openIndexID(int pid = -1);
    void goToPage(int page, int part);
    void openSora(int num);

    void nextPage();
    void prevPage();

    void nextUnit();
    void prevUnit();

    bool hasNext();
    bool hasPrev();

protected:
    void openID(int id = -1);
    void openPage(int page, int part =1);

    QAbstractItemModel *indexModel();
    QAbstractItemModel *topIndexModel();

protected:
    void connected();
    void getBookInfo();
    int getPageNumber(int soraNumber, int ayaNumber=1);
    int getSoraAyatCount(int sora);
    void firstSoraAndAya(int page);

protected:
    QuranQuery *m_quranQuery;
    QuranTextFormat *m_formatter;
};

#endif // QURANDBHANDLER_H
