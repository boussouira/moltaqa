#include "simpledbhandler.h"

simpleDBHandler::simpleDBHandler()
{
}
QString simpleDBHandler::page(int pid)
{
    m_bookQuery->exec(QString("SELECT id, nass, part, page from %1 WHERE id = %2 ")
                      .arg(m_bookInfo->bookTable()).arg(pid));
    if(m_bookQuery->next())
        return m_bookQuery->value(1).toString();
}

QAbstractItemModel *simpleDBHandler::indexModel()
{
    m_bookInfo->debug();
    BookIndexNode *rootNode = new BookIndexNode();
//    BookIndexNode *firstNode = new BookIndexNode(QObject::trUtf8("الفهرس")
//                                                 ,0);

//    rootNode->appendChild(firstNode);

    m_bookQuery->exec(QString("SELECT id, tit, lvl, sub FROM %1 ORDER BY id")
                      .arg(m_bookInfo->titleTable()));
    while(m_bookQuery->next())
    {
        int tid = m_bookQuery->value(0).toInt();
        int level = m_bookQuery->value(2).toInt();
        BookIndexNode *firstChild = new BookIndexNode(m_bookQuery->value(1).toString(), tid);
        BookIndexNode *parent = getNodeByDepth(rootNode, level);

        parent->appendChild(firstChild);
    }

    m_indexModel->setRootNode(rootNode);
    return m_indexModel;
}

BookIndexNode *simpleDBHandler::getNodeByDepth(BookIndexNode *pNode, int pDepth)
{
    BookIndexNode *n = pNode;

    while(--pDepth > 0) {
        n = n->childList()->last();
    }
    return n;
}

void simpleDBHandler::getBookInfo()
{
    QStringList tables = m_bookDB.tables();
    foreach(QString ta, tables) {
        if( ta.contains(QRegExp("(t[0-9]+)")) )
            m_bookInfo->setTitleTable(ta);
        else if( ta.contains(QRegExp("(b[0-9]+)")) )
            m_bookInfo->setBookTable(ta);
    }

    m_bookQuery->exec(QString("SELECT MAX(part), MAX(page) from %1 ")
                      .arg(m_bookInfo->bookTable()));
    if(m_bookQuery->next()) {
        int parts = m_bookQuery->value(0).toInt();
        int pages = m_bookQuery->value(1).toInt();

        if(parts == 1) {
            m_bookInfo->setPartsCount(parts);
            m_bookInfo->setPagesCount(pages);
        } else if(parts > 1) {
            m_bookInfo->setPartsCount(parts);
            for(int i=1;i<=parts;i++) {
                m_bookQuery->exec(QString("SELECT MAX(page) from %1 WHERE part = %2 ")
                                  .arg(m_bookInfo->bookTable()).arg(i));
                if(m_bookQuery->next())
                    m_bookInfo->setPagesCount(m_bookQuery->value(0).toInt(), i);
            }
        }
    }

    m_bookQuery->exec(QString("SELECT bk from Main"));
    if(m_bookQuery->next())
        m_bookInfo->setBookName(m_bookQuery->value(0).toString());
}
