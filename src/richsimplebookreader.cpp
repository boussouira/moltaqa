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
}

RichSimpleBookReader::~RichSimpleBookReader()
{
}

void RichSimpleBookReader::connected()
{
    RichBookReader::connected();
}

void RichSimpleBookReader::goToPage(int pid)
{
    SimpleQuery simpleQuery(m_bookDB, m_bookInfo);

    m_textFormat->start();

    int id;
    if(pid == -1) // First page
        id = m_bookInfo->firstID;
    else if(pid == -2) //Last page
        id = m_bookInfo->lastID;
    else // The given page id
        id = pid;

    if(id >= m_currentPage->pageID)
        simpleQuery.nextPage(id);
    else
        simpleQuery.prevPage(id);

    if(simpleQuery.next()) {
        QString pageText = QString::fromUtf8(qUncompress(simpleQuery.value(1).toByteArray()));

        m_currentPage->pageID = simpleQuery.value(0).toInt();
        m_currentPage->part = simpleQuery.value(2).toInt();
        m_currentPage->page = simpleQuery.value(3).toInt();

        if(m_query && m_highlightPageID == m_currentPage->pageID)
            m_textFormat->insertText(Utils::highlightText(pageText, m_query, false));
        else
            m_textFormat->insertText(pageText);
    }

    m_currentPage->titleID = getPageTitleID(m_currentPage->pageID);

    m_libraryManager->getShoroohPages(m_bookInfo, m_currentPage);

    m_textFormat->done();

    emit textChanged();
}

void RichSimpleBookReader::goToPage(int page, int part)
{
    SimpleQuery simpleQuery(m_bookDB, m_bookInfo);

    simpleQuery.page(page, part);
    if(simpleQuery.next())
        goToPage(simpleQuery.value(0).toInt());
}

void RichSimpleBookReader::goToHaddit(int hadditNum)
{
    SimpleQuery simpleQuery(m_bookDB, m_bookInfo);

    int page = simpleQuery.getHaddithPage(hadditNum);
    if(page)
        goToPage(page);
}

BookIndexModel *RichSimpleBookReader::indexModel()
{
    m_indexModel = new BookIndexModel();
    BookIndexNode *rootNode = new BookIndexNode();

    childTitles(rootNode, 0);

    m_indexModel->setRootNode(rootNode);

    return m_indexModel;
}

BookIndexModel * RichSimpleBookReader::topIndexModel()
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
