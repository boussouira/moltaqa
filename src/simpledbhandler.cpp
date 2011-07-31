#include "simpledbhandler.h"
#include "abstractdbhandler.h"
#include "bookinfo.h"
#include "bookindexmodel.h"
#include "bookindexnode.h"
#include "bookexception.h"
#include "simpletextformat.h"
#include "libraryinfo.h"
#include "sqlitesimplequery.h"

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
    delete m_simpleQuery;
}

void SimpleDBHandler::connected()
{
    if(m_connetionInfo->type() == LibraryInfo::SQLITE)
        m_simpleQuery = new SqliteSimpleQuery(m_bookDB, m_bookInfo);
    else
        throw BookException(tr("لم يمكن تحديد نوع الكتاب"));

    AbstractDBHandler::connected();
}

void SimpleDBHandler::openID(int pid)
{
    m_textFormat->start();

    int id;
    if(pid == -1) // First page
        id = m_bookInfo->firstID();
    else if(pid == -2) //Last page
        id = m_bookInfo->lastID();
    else // The given page id
        id = pid;

    if(id >= m_bookInfo->currentID())
        m_simpleQuery->nextPage(id);
    else
        m_simpleQuery->prevPage(id);

    if(m_simpleQuery->next()) {
        m_textFormat->insertText(m_simpleQuery->value(1).toString());
        m_bookInfo->setCurrentID(m_simpleQuery->value(0).toInt());
        m_bookInfo->setCurrentPage(m_simpleQuery->value(3).toInt());
        m_bookInfo->setCurrentPart(m_simpleQuery->value(2).toInt());
    }

    m_textFormat->done();
}

void SimpleDBHandler::openPage(int page, int part)
{
    m_simpleQuery->page(page, part);
    if(m_simpleQuery->next())
        openID(m_simpleQuery->value(0).toInt());
}

QAbstractItemModel *SimpleDBHandler::indexModel()
{
    BookIndexNode *rootNode = new BookIndexNode();

    m_simpleQuery->index();
    while(m_simpleQuery->next())
    {
        int tid = m_simpleQuery->value(0).toInt();
        int level = m_simpleQuery->value(2).toInt();
        BookIndexNode *firstChild = new BookIndexNode(m_simpleQuery->value(1).toString(), tid);
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

    m_simpleQuery->exec(QString("SELECT MAX(part), MIN(page), MAX(page), MIN(id), MAX(id) from %1 ")
                      .arg(m_bookInfo->bookTable()));
    if(m_simpleQuery->next()) {
        bool ok;
        int parts = m_simpleQuery->value(0).toInt(&ok);
        if(!ok)
            parts = maxPartNum();

        m_bookInfo->setFirstID(m_simpleQuery->value(3).toInt());
        m_bookInfo->setLastID(m_simpleQuery->value(4).toInt());

        if(parts == 1) {
            m_bookInfo->setPartsCount(parts);
            m_bookInfo->setFirstPage(m_simpleQuery->value(1).toInt());
            m_bookInfo->setLastPage(m_simpleQuery->value(2).toInt());
        } else if(parts > 1) {
            m_bookInfo->setPartsCount(parts);
            for(int i=1;i<=parts;i++) {
                m_simpleQuery->exec(QString("SELECT MIN(page), MAX(page) from %1 WHERE part = %2 ")
                                  .arg(m_bookInfo->bookTable()).arg(i));
                if(m_simpleQuery->next()) {
                    m_bookInfo->setFirstPage(m_simpleQuery->value(0).toInt(), i);
                    m_bookInfo->setLastPage(m_simpleQuery->value(1).toInt(), i);
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

void SimpleDBHandler::nextPage()
{
    if(hasNext())
        openID(m_bookInfo->currentID()+1);
}

void SimpleDBHandler::prevPage()
{
    if(hasPrev())
        openID(m_bookInfo->currentID()-1);
}

bool SimpleDBHandler::hasNext()
{
    return (m_bookInfo->currentID() < m_bookInfo->lastID());
}

bool SimpleDBHandler::hasPrev()
{
    return (m_bookInfo->currentID() > m_bookInfo->firstID());
}
