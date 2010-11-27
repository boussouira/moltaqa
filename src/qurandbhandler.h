#ifndef QURANDBHANDLER_H
#define QURANDBHANDLER_H

#include "abstractdbhandler.h"
#include "qurantextformat.h"

class QuranDBHandler : public AbstractDBHandler
{
public:
    QuranDBHandler();
    ~QuranDBHandler();

    void openID(int id = -1);
    void openPage(int page, int part =1);
    void openIndexID(int pid = -1);
    void openSora(int num);
    void nextPage();
    void prevPage();

    void nextUnit();
    void prevUnit();

    bool hasNext();
    bool hasPrev();

    QAbstractItemModel *indexModel();
protected:
    void getBookInfo();
    int getPageNumber(int soraNumber, int ayaNumber=1);
    int getSoraAyatCount(int sora);
    void firstSoraAndAya(int page);
};

#endif // QURANDBHANDLER_H
