#include "bookslistnode.h"

BooksListNode::BooksListNode(Type pType, QString pTitle, QString pAuth, int pId)
{
    this->type = pType;
    this->title = pTitle;
    this->authorName = pAuth;
    this->nodeID = pId;
    this->parent = 0;
}

BooksListNode::~BooksListNode()
{
    qDeleteAll(children);
}

void BooksListNode::appendChild(BooksListNode *pNode)
{
    pNode->parent = this;
    this->children.append(pNode);
}

void BooksListNode::setID(int pId)
{
    this->nodeID = pId;
}
