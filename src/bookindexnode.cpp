#include "bookindexnode.h"

BookIndexNode::BookIndexNode(): m_parent(0)
{
}

BookIndexNode::BookIndexNode(QString title, int id): m_parent(0), m_id(id), m_title(title)
{
}

void BookIndexNode::appendChild(BookIndexNode *pNode)
{
    pNode->setParent(this);
    this->m_childNode.append(pNode);
}

BookIndexNode::~BookIndexNode()
{
    qDeleteAll(m_childNode);
}
