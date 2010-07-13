#ifndef SIMPLEDBHANDLER_H
#define SIMPLEDBHANDLER_H

#include "abstractdbhandler.h"
#include "simpletextformat.h"

class SimpleDBHandler : public AbstractDBHandler
{
public:
    SimpleDBHandler();
    ~SimpleDBHandler();

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
