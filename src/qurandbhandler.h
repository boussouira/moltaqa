#ifndef QURANDBHANDLER_H
#define QURANDBHANDLER_H

#include "abstractdbhandler.h"
#include "qurantextformat.h"
#include <qsqlerror.h>
#include <qdebug.h>

class QuranDBHandler : public AbstractDBHandler
{
public:
    QuranDBHandler();
    ~QuranDBHandler();

    QString openID(int id = -1);
    QString openPage(int page, int part =1);
    QString openIndexID(int pid = -1);
    QString openSora(int num);
    QString nextPage();
    QString prevPage();

    QString nextUnit();
    QString prevUnit();

    bool hasNext();
    bool hasPrev();

    QAbstractItemModel *indexModel();
protected:
    void getBookInfo();
    int getPageNumber(int soraNumber, int ayaNumber=1);
    int getSoraAyatCount(int sora);
    void firstSoraAndAya(int page);
    QuranTextFormat *m_quranFormat;

};

#endif // QURANDBHANDLER_H
