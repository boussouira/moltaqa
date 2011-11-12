#include "bookindexnode.h"

BookIndexNode::BookIndexNode(): parentNode(0)
{
}

BookIndexNode::BookIndexNode(QString _title, int _id) :
    parentNode(0),
    id(_id),
    title(_title)
{
}

void BookIndexNode::appendChild(BookIndexNode *pNode)
{
    pNode->parentNode = this;
    childs.append(pNode);
}

BookIndexNode::~BookIndexNode()
{
    qDeleteAll(childs);
}
