#ifndef BOOKSLISTNODE_H
#define BOOKSLISTNODE_H

#include <qstring.h>
#include <qlist.h>
#include "librarybook.h"

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
    LibraryBook::Type bookType;
    QString title;
    QString authorName;
    QString authorDeath;
    int authorDeathYear;
    QString infoToolTip;
    int id;
    int order;
    BooksListNode *parentNode;
    QList<BooksListNode *> childrenNode;
};

#endif // BOOKSLISTNODE_H
