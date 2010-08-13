#ifndef BOOKINDEXNODE_H
#define BOOKINDEXNODE_H

#include <qstring.h>
#include <qlist.h>

class BookIndexNode
{
public:
    BookIndexNode();
    BookIndexNode(QString title, int id);
    ~BookIndexNode();
    void appendChild(BookIndexNode *pNode);
    void setTitle(const QString &title) {m_title = title;}
    QString title() { return m_title; }
    void setID(int id) {m_id = id;}
    int id(){ return m_id; }
    BookIndexNode *parent() { return m_parent; }
    void setParent(BookIndexNode *parent) { m_parent = parent; }
    QList<BookIndexNode *> *childList() {return &m_childNode; }

protected:
    BookIndexNode *m_parent;
    int m_id;
    QString m_title;
    QList<BookIndexNode *> m_childNode;
};

#endif // BOOKINDEXNODE_H
