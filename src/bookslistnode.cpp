#include "bookslistnode.h"

BooksListNode::BooksListNode(Type pType, QString pTitle, QString pAuth, int pId) :
    m_type(pType), m_title(pTitle), m_authorName(pAuth), m_nodeID(pId)
{
    this->m_parentNode = 0;
}

BooksListNode::~BooksListNode()
{
    qDeleteAll(m_childrenNode);
    m_childrenNode.clear();
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
