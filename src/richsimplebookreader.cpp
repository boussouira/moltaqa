#include "richsimplebookreader.h"
#include "abstractbookreader.h"
#include "librarybook.h"
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

RichSimpleBookReader::RichSimpleBookReader(QObject *parent) : RichBookReader(parent)
{
    m_textFormat = new SimpleTextFormat();
    m_simpleQuery = 0;
}

RichSimpleBookReader::~RichSimpleBookReader()
{
    if(m_simpleQuery)
        delete m_simpleQuery;
}

void RichSimpleBookReader::connected()
{
    m_simpleQuery = new SimpleQuery(m_bookDB, m_bookInfo);
    AbstractBookReader::connected();
}

void RichSimpleBookReader::goToPage(int pid)
{
    m_textFormat->start();

    int id;
    if(pid == -1) // First page
        id = m_bookInfo->firstID;
    else if(pid == -2) //Last page
        id = m_bookInfo->lastID;
    else // The given page id
        id = pid;

    if(id >= m_currentPage->pageID)
        m_simpleQuery->nextPage(id);
    else
        m_simpleQuery->prevPage(id);

    if(m_simpleQuery->next()) {
        m_textFormat->insertText(QString::fromUtf8(qUncompress(m_simpleQuery->value(1).toByteArray())));
        m_currentPage->pageID = m_simpleQuery->value(0).toInt();
        m_currentPage->part = m_simpleQuery->value(2).toInt();
        m_currentPage->page = m_simpleQuery->value(3).toInt();
    }

    m_libraryManager->getShoroohPages(m_bookInfo, m_currentPage);

    m_textFormat->done();

    emit textChanged();
}

void RichSimpleBookReader::goToPage(int page, int part)
{
    m_simpleQuery->page(page, part);
    if(m_simpleQuery->next())
        goToPage(m_simpleQuery->value(0).toInt());
}

void RichSimpleBookReader::goToHaddit(int hadditNum)
{
    int page = m_simpleQuery->getHaddithPage(hadditNum);

    if(page)
        goToPage(page);
}

QAbstractItemModel *RichSimpleBookReader::indexModel()
{
    BookIndexNode *rootNode = new BookIndexNode();

    childTitles(rootNode, 0);

    m_indexModel->setRootNode(rootNode);

    return m_indexModel;
}

QAbstractItemModel * RichSimpleBookReader::topIndexModel()
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

void RichSimpleBookReader::childTitles(BookIndexNode *parentNode, int tid)
{
    QSqlQuery query(m_bookDB);
    query.exec(QString("SELECT id, parentID, pageID, title FROM bookIndex "
                          "WHERE parentID = %1 ORDER BY id").arg(tid));
    while(!m_stopModelLoad && query.next()){

        BookIndexNode *catNode = new BookIndexNode(query.value(3).toString(),
                                                   query.value(2).toInt());
        childTitles(catNode, query.value(0).toInt());
        parentNode->appendChild(catNode);
    }
}
