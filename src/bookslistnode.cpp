#include "bookslistnode.h"

BooksListNode::BooksListNode(Type pType, QString pTitle, QString pAuth, int pId) :
    type(pType), title(pTitle), authorName(pAuth), id(pId), parentNode(0)
{
    authorDeathYear = 0;
    order = -1;
    bookType = LibraryBook::NormalBook;
}

BooksListNode::~BooksListNode()
{
    qDeleteAll(childrenNode);
    childrenNode.clear();
}

void BooksListNode::appendChild(BooksListNode *node)
{
    node->parentNode = this;
    childrenNode.append(node);
}

void BooksListNode::appendChild(int index, BooksListNode *node)
{
    node->parentNode = this;
    childrenNode.insert(index, node);
}
