#ifndef SIMPLEDBHANDLER_H
#define SIMPLEDBHANDLER_H

#include "abstractdbhandler.h"

class BookIndexNode;
class SimpleQuery;

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
    void connected();
    BookIndexNode *getNodeByDepth(BookIndexNode *pNode, int pDepth);
    void getBookInfo();
    int maxPartNum();

protected:
    SimpleQuery *m_simpleQuery;
};

#endif // SIMPLEDBHANDLER_H
