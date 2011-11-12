#ifndef BOOKINDEXNODE_H
#define BOOKINDEXNODE_H

#include <qstring.h>
#include <qlist.h>

class BookIndexNode
{
public:
    BookIndexNode();
    BookIndexNode(QString _title, int _id);
    ~BookIndexNode();
    void appendChild(BookIndexNode *pNode);

    BookIndexNode *parentNode;
    int id;
    QString title;
    QList<BookIndexNode *> childs;
};

#endif // BOOKINDEXNODE_H
