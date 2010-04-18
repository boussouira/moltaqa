#ifndef BOOKSLISTNODE_H
#define BOOKSLISTNODE_H

#include <QString>
#include <QList>

class BooksListNode
{
public:
    enum Type { Root, Categorie, Book };

public:
    BooksListNode(Type pType, QString pTitle = "", QString pAuth = "", int pId = 0);
    ~BooksListNode();
    void appendChild(BooksListNode *pNode);
    void setId(int pId);

public:
    Type type;
    QString title;
    QString authorName;
    int bid;
    BooksListNode *parent;
    QList<BooksListNode *> children;
};

#endif // BOOKSLISTNODE_H
