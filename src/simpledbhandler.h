#ifndef SIMPLEDBHANDLER_H
#define SIMPLEDBHANDLER_H

#include "abstractdbhandler.h"
#include "simpletextformat.h"
#include "bookindexnode.h"

class SimpleDBHandler : public AbstractDBHandler
{
public:
    SimpleDBHandler();
    ~SimpleDBHandler();

    void openID(int id = -1);
    void openPage(int page, int part =1);
    void nextPage();
    void prevPage();

    bool hasNext();
    bool hasPrev();

    QAbstractItemModel *indexModel();

protected:
    BookIndexNode *getNodeByDepth(BookIndexNode *pNode, int pDepth);
    void getBookInfo();
    int maxPartNum();
};

#endif // SIMPLEDBHANDLER_H
