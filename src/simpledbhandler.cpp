#include "simpledbhandler.h"
#include "abstractdbhandler.h"
#include "bookinfo.h"
#include "bookindexmodel.h"
#include "bookexception.h"

#include <qsqlquery.h>
#include <qstringlist.h>
#include <qdebug.h>

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
    m_textFormat->clearText();

    int id;
    if(pid == -1) // First page
        id = m_bookInfo->firstID();
    else if(pid == -2) //Last page
        id = m_bookInfo->lastID();
    else // The given page id
        id = pid;
    if(id >= m_bookInfo->currentID())
        m_bookQuery.exec(QString("SELECT id, nass, part, page from %1 "
                                  "WHERE id >= %2 ORDER BY id ASC LIMIT 1")
                          .arg(m_bookInfo->bookTable()).arg(id));
    else
        m_bookQuery.exec(QString("SELECT id, nass, part, page from %1 "
                                  "WHERE id <= %2 ORDER BY id DESC LIMIT 1")
                          .arg(m_bookInfo->bookTable()).arg(id));
    if(m_bookQuery.next()) {
        m_textFormat->insertText(m_bookQuery.value(1).toString());
        m_bookInfo->setCurrentID(m_bookQuery.value(0).toInt());
        m_bookInfo->setCurrentPage(m_bookQuery.value(3).toInt());
        m_bookInfo->setCurrentPart(m_bookQuery.value(2).toInt());
    }

    return m_textFormat->getText();
}

QString SimpleDBHandler::openPage(int page, int part)
{
    m_bookQuery.exec(QString("SELECT id FROM %1 WHERE page >= %2 AND part = %3"
                              " ORDER BY id ASC LIMIT 1")
                      .arg(m_bookInfo->bookTable()).arg(page).arg(part));
    if(m_bookQuery.next())
        return openID(m_bookQuery.value(0).toInt());
    else
        return QString();

}

QAbstractItemModel *SimpleDBHandler::indexModel()
{
    BookIndexNode *rootNode = new BookIndexNode();

    m_bookQuery.exec(QString("SELECT id, tit, lvl, sub FROM %1 ORDER BY id")
                      .arg(m_bookInfo->titleTable()));
    while(m_bookQuery.next())
    {
        int tid = m_bookQuery.value(0).toInt();
        int level = m_bookQuery.value(2).toInt();
        BookIndexNode *firstChild = new BookIndexNode(m_bookQuery.value(1).toString(), tid);
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
        if(!n->childList()->isEmpty())
            n = n->childList()->last();
    }
    return n;
}

void SimpleDBHandler::getBookInfo()
{
    QStringList tables = m_bookDB.tables();

    m_bookInfo->setTitleTable(tables.at(tables.indexOf(QRegExp("(t[0-9]+)"))));
    m_bookInfo->setBookTable(tables.at(tables.indexOf(QRegExp("(b[0-9]+)"))));

    m_bookQuery.exec(QString("SELECT MAX(part), MIN(page), MAX(page), MIN(id), MAX(id) from %1 ")
                      .arg(m_bookInfo->bookTable()));
    if(m_bookQuery.next()) {
        bool ok;
        int parts = m_bookQuery.value(0).toInt(&ok);
        if(!ok)
            parts = maxPartNum();

        m_bookInfo->setFirstID(m_bookQuery.value(3).toInt());
        m_bookInfo->setLastID(m_bookQuery.value(4).toInt());

        if(parts == 1) {
            m_bookInfo->setPartsCount(parts);
            m_bookInfo->setFirstPage(m_bookQuery.value(1).toInt());
            m_bookInfo->setLastPage(m_bookQuery.value(2).toInt());
        } else if(parts > 1) {
            m_bookInfo->setPartsCount(parts);
            for(int i=1;i<=parts;i++) {
                m_bookQuery.exec(QString("SELECT MIN(page), MAX(page) from %1 WHERE part = %2 ")
                                  .arg(m_bookInfo->bookTable()).arg(i));
                if(m_bookQuery.next()) {
                    m_bookInfo->setFirstPage(m_bookQuery.value(0).toInt(), i);
                    m_bookInfo->setLastPage(m_bookQuery.value(1).toInt(), i);
                }
            }
        }
    }
}

int SimpleDBHandler::maxPartNum()
{
    QSqlQuery query(m_bookDB);
    query.exec(QString("SELECT part FROM %1 ORDER BY id DESC")
               .arg(m_bookInfo->bookTable()));
    while(query.next()) {
        QString val = query.value(0).toString().trimmed();
        bool ok;
        int parts = val.toInt(&ok);
        if(ok){
            return parts;
        }
    }

    throw BookException(tr("لم يمكن تحديد عدد أجزاء الكتاب"), m_bookInfo->bookPath());
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
