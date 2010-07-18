#include "simpledbhandler.h"
#include "abstractdbhandler.h"
#include "bookinfo.h"
#include "bookindexmodel.h"

#include <qdebug.h>
#include <qsqlquery.h>
#include <qstringlist.h>

SimpleDBHandler::SimpleDBHandler()
{
    m_textFormat = new SimpleTextFormat();
}

SimpleDBHandler::~SimpleDBHandler()
{
    delete m_textFormat;
}

QString SimpleDBHandler::openID(int pid)
{
    int id;
    if(pid == -1) // First page
        id = m_bookInfo->firstID();
    else if(pid == -2) //Last page
        id = m_bookInfo->lastID();
    else // The given page id
        id = pid;
    if(id >= m_bookInfo->currentID())
        m_bookQuery->exec(QString("SELECT id, nass, part, page from %1 "
                                  "WHERE id >= %2 ORDER BY id ASC LIMIT 1")
                          .arg(m_bookInfo->bookTable()).arg(id));
    else
        m_bookQuery->exec(QString("SELECT id, nass, part, page from %1 "
                                  "WHERE id <= %2 ORDER BY id DESC LIMIT 1")
                          .arg(m_bookInfo->bookTable()).arg(id));
    if(m_bookQuery->next()) {
        m_textFormat->setText(m_bookQuery->value(1).toString());
        m_bookInfo->setCurrentID(m_bookQuery->value(0).toInt());
        m_bookInfo->setCurrentPage(m_bookQuery->value(3).toInt());
        m_bookInfo->setCurrentPart(m_bookQuery->value(2).toInt());
    }
//    qDebug("CURRENT PAGE: %d", m_bookInfo->currentID());
    return m_textFormat->formatText();
}

QString SimpleDBHandler::openPage(int page, int part)
{
    m_bookQuery->exec(QString("SELECT id FROM %1 WHERE page >= %2 AND part = %3"
                              " ORDER BY id ASC LIMIT 1")
                      .arg(m_bookInfo->bookTable()).arg(page).arg(part));
    if(m_bookQuery->next())
        return openID(m_bookQuery->value(0).toInt());
    else
        return QString();

}

QAbstractItemModel *SimpleDBHandler::indexModel()
{
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

BookIndexNode *SimpleDBHandler::getNodeByDepth(BookIndexNode *pNode, int pDepth)
{
    BookIndexNode *n = pNode;

    while(--pDepth > 0) {
        n = n->childList()->last();
    }
    return n;
}

void SimpleDBHandler::getBookInfo()
{
    QStringList tables = m_bookDB.tables();
    foreach(QString ta, tables) {
        if( ta.contains(QRegExp("(t[0-9]+)")) )
            m_bookInfo->setTitleTable(ta);
        else if( ta.contains(QRegExp("(b[0-9]+)")) )
            m_bookInfo->setBookTable(ta);
    }

    m_bookQuery->exec(QString("SELECT MAX(part), MIN(page), MAX(page), MIN(id), MAX(id) from %1 ")
                      .arg(m_bookInfo->bookTable()));
    if(m_bookQuery->next()) {
        int parts = m_bookQuery->value(0).toInt();

        m_bookInfo->setFirstID(m_bookQuery->value(3).toInt());
        m_bookInfo->setLastID(m_bookQuery->value(4).toInt());

        if(parts == 1) {
            m_bookInfo->setPartsCount(parts);
            m_bookInfo->setFirstPage(m_bookQuery->value(1).toInt());
            m_bookInfo->setLastPage(m_bookQuery->value(2).toInt());
        } else if(parts > 1) {
            m_bookInfo->setPartsCount(parts);
            for(int i=1;i<=parts;i++) {
                m_bookQuery->exec(QString("SELECT MIN(page), MAX(page) from %1 WHERE part = %2 ")
                                  .arg(m_bookInfo->bookTable()).arg(i));
                if(m_bookQuery->next()) {
                    m_bookInfo->setFirstPage(m_bookQuery->value(0).toInt(), i);
                    m_bookInfo->setLastPage(m_bookQuery->value(1).toInt(), i);
                }
            }
        }
    }
}

QString SimpleDBHandler::nextPage()
{
    return hasNext() ? this->openID(m_bookInfo->currentID()+1) : QString();
}

QString SimpleDBHandler::prevPage()
{
    return hasPrev() ? this->openID(m_bookInfo->currentID()-1) : QString();
}

bool SimpleDBHandler::hasNext()
{
    return (m_bookInfo->currentID() < m_bookInfo->lastID());
}

bool SimpleDBHandler::hasPrev()
{
    return (m_bookInfo->currentID() > m_bookInfo->firstID());
}
