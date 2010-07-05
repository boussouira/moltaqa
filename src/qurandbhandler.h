#ifndef QURANDBHANDLER_H
#define QURANDBHANDLER_H

#include "abstractdbhandler.h"
#include "qurantextformat.h"

class QuranDBHandler : public AbstractDBHandler
{
public:
    QuranDBHandler();

    QString page(int pid);
    QString nextPage(){}
    QString prevPage(){}

    void nextUnit(){}
    void prevUnit(){}

    bool hasNext(){}
    bool hasPrev(){}

    QAbstractItemModel *indexModel();
    QString getFormattedPage(){}
protected:
    void getBookInfo();
    int getPageNumber(int soraNumber);
    QuranTextFormat *m_quranFormat;

};

#endif // QURANDBHANDLER_H
