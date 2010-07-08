#ifndef SIMPLEDBHANDLER_H
#define SIMPLEDBHANDLER_H

#include "abstractdbhandler.h"
#include "simpletextformat.h"

class simpleDBHandler : public AbstractDBHandler
{
public:
    simpleDBHandler();
    ~simpleDBHandler();

    QString page(int pid = -1);
    QString nextPage();
    QString prevPage();

    bool hasNext();
    bool hasPrev();

    QAbstractItemModel *indexModel();

protected:
    BookIndexNode *getNodeByDepth(BookIndexNode *pNode, int pDepth);
    void getBookInfo();
    SimpleTextFormat *m_textFormat;
};

#endif // SIMPLEDBHANDLER_H
