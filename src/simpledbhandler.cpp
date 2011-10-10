#include "simpledbhandler.h"
#include "abstractdbhandler.h"
#include "bookinfo.h"
#include "bookindexmodel.h"
#include "bookindexnode.h"
#include "bookexception.h"
#include "simpletextformat.h"
#include "libraryinfo.h"
#include "simplequery.h"

#include <qsqlquery.h>
#include <qstringlist.h>
#include <qdebug.h>
#include <qdatetime.h>

SimpleDBHandler::SimpleDBHandler()
{
    m_textFormat = new SimpleTextFormat();
    m_fastIndex = true;
    m_simpleQuery = 0;
}

SimpleDBHandler::~SimpleDBHandler()
{
    delete m_textFormat;

    if(m_simpleQuery)
        delete m_simpleQuery;
}

void SimpleDBHandler::connected()
{
    m_simpleQuery = new SimpleQuery(m_bookDB, m_bookInfo);
    AbstractDBHandler::connected();
}

void SimpleDBHandler::openID(int pid)
{
    m_textFormat->start();

    int id;
    if(pid == -1) // First page
        id = m_bookInfo->firstID;
    else if(pid == -2) //Last page
        id = m_bookInfo->lastID;
    else // The given page id
        id = pid;

    if(id >= m_bookInfo->currentPage.pageID)
        m_simpleQuery->nextPage(id);
    else
        m_simpleQuery->prevPage(id);

    if(m_simpleQuery->next()) {
        m_textFormat->insertText(QString::fromUtf8(qUncompress(m_simpleQuery->value(1).toByteArray())));
        m_bookInfo->currentPage.pageID = m_simpleQuery->value(0).toInt();
        m_bookInfo->currentPage.part = m_simpleQuery->value(2).toInt();
        m_bookInfo->currentPage.page = m_simpleQuery->value(3).toInt();
    }

    m_libraryManager->getShoroohPages(m_bookInfo);

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

    childTitles(rootNode, 0);

    m_indexModel->setRootNode(rootNode);

    return m_indexModel;
}

QAbstractItemModel * SimpleDBHandler::topIndexModel()
{
    BookIndexModel *indexModel = new BookIndexModel();
    BookIndexNode *rootNode = new BookIndexNode();

    QSqlQuery query(m_bookDB);
    query.exec(QString("SELECT id, parentID, pageID, title FROM bookIndex "
                          "WHERE parentID = %1 ORDER BY id").arg(0));
    while(query.next())
    {
        BookIndexNode *catNode = new BookIndexNode(query.value(3).toString(),
                                                   query.value(2).toInt());
        rootNode->appendChild(catNode);
    }

    indexModel->setRootNode(rootNode);

    return indexModel;
}

void SimpleDBHandler::childTitles(BookIndexNode *parentNode, int tid)
{
    QSqlQuery query(m_bookDB);
    query.exec(QString("SELECT id, parentID, pageID, title FROM bookIndex "
                          "WHERE parentID = %1 ORDER BY id").arg(tid));
    while(query.next())
    {
        BookIndexNode *catNode = new BookIndexNode(query.value(3).toString(),
                                                   query.value(2).toInt());
        childTitles(catNode, query.value(0).toInt());
        parentNode->appendChild(catNode);
    }
}

void SimpleDBHandler::getBookInfo()
{
    m_bookInfo->textTable = "bookPages";
    m_bookInfo->indexTable = "bookIndex";

    if(!m_bookInfo->haveInfo()) {
        m_simpleQuery->exec(QString("SELECT MAX(partNum), MIN(id), MAX(id) from %1 ").arg(m_bookInfo->textTable));
        if(m_simpleQuery->next()) {
            bool ok;
            int parts = m_simpleQuery->value(0).toInt(&ok);
            if(!ok)
                qWarning("Can't get parts count");

            m_bookInfo->partsCount = parts;
            m_bookInfo->firstID = m_simpleQuery->value(1).toInt();
            m_bookInfo->lastID = m_simpleQuery->value(2).toInt();
        }

        m_libraryManager->updateBookMeta(m_bookInfo, false);
    }
}

void SimpleDBHandler::nextPage()
{
    if(hasNext())
        openID(m_bookInfo->currentPage.pageID+1);
}

void SimpleDBHandler::prevPage()
{
    if(hasPrev())
        openID(m_bookInfo->currentPage.pageID-1);
}

bool SimpleDBHandler::hasNext()
{
    return (m_bookInfo->currentPage.pageID < m_bookInfo->lastID);
}

bool SimpleDBHandler::hasPrev()
{
    return (m_bookInfo->currentPage.pageID > m_bookInfo->firstID);
}

void SimpleDBHandler::goToSora(int sora, int aya)
{
    //Doesn't do any thing
    Q_UNUSED(sora);
    Q_UNUSED(aya);
}

void SimpleDBHandler::goToHaddit(int hadditNum)
{
    int page = m_simpleQuery->getHaddithPage(hadditNum);

    if(page)
        openID(page);
}

