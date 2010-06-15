#include "bookslistnode.h"

BooksListNode::BooksListNode(Type pType, QString pTitle, QString pAuth, int pId)
{
    this->m_type = pType;
    this->m_title = pTitle;
    this->m_authorName = pAuth;
    this->m_nodeID = pId;
    this->m_parentNode = 0;
}

BooksListNode::~BooksListNode()
{
    qDeleteAll(m_childrenNode);
}

void BooksListNode::appendChild(BooksListNode *pNode)
{
    pNode->m_parentNode = this;
    this->m_childrenNode.append(pNode);
}

void BooksListNode::setID(int pId)
{
    this->m_nodeID = pId;
}
