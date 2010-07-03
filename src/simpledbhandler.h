#ifndef SIMPLEDBHANDLER_H
#define SIMPLEDBHANDLER_H

#include "abstractdbhandler.h"
#include "simpletextformat.h"

class simpleDBHandler : public AbstractDBHandler
{
public:
    simpleDBHandler();

    QString page(int pid);
    QString nextPage(){}
    QString prevPage(){}

    void nextUnit(){}
    void prevUnit(){}

    bool canGoNext(){}
    bool canGoPrev(){}

    QAbstractItemModel *indexModel();
    QString getFormattedPage(){}

protected:
    BookIndexNode *getNodeByDepth(BookIndexNode *pNode, int pDepth);
    void getBookInfo();
    SimpleTextFormat *m_textFormat;
};

#endif // SIMPLEDBHANDLER_H
