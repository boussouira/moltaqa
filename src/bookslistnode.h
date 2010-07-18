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
    void setID(int pId);
    void setInfoToolTip(const QString &info) {this->m_infoToolTip = info;}

    Type getNodeType() const { return m_type; }
    BooksListNode *parentNode() const { return m_parentNode; }
    int getID() const { return m_nodeID; }
    QList<BooksListNode *> childrenList() const { return m_childrenNode; }
    QString getTitle() const { return m_title; }
    QString getAuthorName() const { return m_authorName; }
    QString getInfoToolTip() { return this->m_infoToolTip; }

protected:
    Type m_type;
    QString m_title;
    QString m_authorName;
    QString m_infoToolTip;
    int m_nodeID;
    BooksListNode *m_parentNode;
    QList<BooksListNode *> m_childrenNode;
};

#endif // BOOKSLISTNODE_H
