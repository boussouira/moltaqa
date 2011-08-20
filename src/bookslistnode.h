#ifndef BOOKSLISTNODE_H
#define BOOKSLISTNODE_H

#include <qstring.h>
#include <qlist.h>

class BooksListNode
{
public:
    enum Type { Root, Categorie, Book };

public:
    BooksListNode(Type pType, QString pTitle = "", QString pAuth = "", int pId = 0);
    ~BooksListNode();
    void appendChild(BooksListNode *pNode);
    void appendChild(int index, BooksListNode *pNode);

public:
    Type type;
    QString title;
    QString authorName;
    QString infoToolTip;
    int id;
    BooksListNode *parentNode;
    QList<BooksListNode *> childrenNode;
};

#endif // BOOKSLISTNODE_H
