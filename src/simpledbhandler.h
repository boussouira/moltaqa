#ifndef SIMPLEDBHANDLER_H
#define SIMPLEDBHANDLER_H

#include "abstractdbhandler.h"

class simpleDBHandler : public AbstractDBHandler
{
public:
    simpleDBHandler();
    QString nextPage(){}
    QString prevPage(){}

    void nextUnit(){}
    void prevUnit(){}

    bool canGoNext(){}
    bool canGoPrev(){}

    QAbstractItemModel *indexModel(){}
    QString getFormattedPage(){}
};

#endif // SIMPLEDBHANDLER_H
