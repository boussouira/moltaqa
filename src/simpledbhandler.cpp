#include "simpledbhandler.h"

simpleDBHandler::simpleDBHandler()
{
}
QAbstractItemModel *simpleDBHandler::indexModel()
{
    BookIndexNode *rootNode = new BookIndexNode();
//    BookIndexNode *firstNode = new BookIndexNode(QObject::trUtf8("الفهرس")
//                                                 ,0);

//    rootNode->appendChild(firstNode);

    m_bookQuery->exec("SELECT id, tit, lvl, sub FROM t1 ORDER BY id");
    while(m_bookQuery->next())
    {
        int tid = m_bookQuery->value(0).toInt();
        int level = m_bookQuery->value(2).toInt();
        BookIndexNode *firstChild = new BookIndexNode(m_bookQuery->value(1).toString(), tid);
        BookIndexNode *parent = getNodeByDepth(rootNode, level);

        parent->appendChild(firstChild);
    }

    m_indexModel->setRootNode(rootNode);
    return m_indexModel;
}

BookIndexNode *simpleDBHandler::getNodeByDepth(BookIndexNode *pNode, int pDepth)
{
    BookIndexNode *n = pNode;

    while(--pDepth > 0) {
        n = n->childList()->last();
    }
    return n;
}
