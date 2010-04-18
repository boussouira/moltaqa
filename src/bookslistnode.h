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
    void setID(int pId);

    Type getNodeType() const { return type; }
    BooksListNode *parentNode() const { return parent; }
    int getID() const { return nodeID; }
    QList<BooksListNode *> childrenList() const { return children; }
    QString getTitle() const { return title; }
    QString getAuthorName() const { return authorName; }

protected:
    Type type;
    QString title;
    QString authorName;
    int nodeID;
    BooksListNode *parent;
    QList<BooksListNode *> children;
};

#endif // BOOKSLISTNODE_H
