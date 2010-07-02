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

    bool canGoNext(){}
    bool canGoPrev(){}

    QAbstractItemModel *indexModel(){}
    QString getFormattedPage(){}
};

#endif // QURANDBHANDLER_H
