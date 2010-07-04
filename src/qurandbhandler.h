#ifndef QURANDBHANDLER_H
#define QURANDBHANDLER_H

#include "abstractdbhandler.h"

class QuranDBHandler : public AbstractDBHandler
{
public:
    QuranDBHandler();

    QString page(int pid){}
    QString nextPage(){}
    QString prevPage(){}

    void nextUnit(){}
    void prevUnit(){}

    bool hasNext(){}
    bool hasPrev(){}

    QAbstractItemModel *indexModel(){}
    QString getFormattedPage(){}
    void getBookInfo(){}
};

#endif // QURANDBHANDLER_H
