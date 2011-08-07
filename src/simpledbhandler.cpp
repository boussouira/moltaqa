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
#include <QTime>

SimpleDBHandler::SimpleDBHandler()
{
    m_textFormat = new SimpleTextFormat();
    m_fastIndex = true;
}

SimpleDBHandler::~SimpleDBHandler()
{
    delete m_textFormat;
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
        m_textFormat->insertText(QString::fromUtf8(qUncompress(m_simpleQuery->value(1).toByteArray())));
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
    QTime time;
    time.start();
    BookIndexNode *rootNode = new BookIndexNode();

    childTitles(rootNode, 0);

    qDebug() << "Load index take:" << time.elapsed() << "ms";
    m_indexModel->setRootNode(rootNode);

    return m_indexModel;
}

QAbstractItemModel * SimpleDBHandler::topIndexModel()
{
    BookIndexModel *indexModel = new BookIndexModel();
    QTime time;
    time.start();
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

    qDebug() << "Load Top index take:" << time.elapsed() << "ms";
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
    // TODO: some books start with part 0
    m_bookInfo->setBookTable("bookPages");
    m_bookInfo->setTitleTable("bookIndex");

    if(!m_bookInfo->haveInfo()) {
        m_simpleQuery->exec(QString("SELECT MAX(partNum), MIN(pageNum), MAX(pageNum), MIN(id), MAX(id) from %1 ")
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
                    m_simpleQuery->exec(QString("SELECT MIN(pageNum), MAX(pageNum) from %1 WHERE partNum = %2 ")
                                        .arg(m_bookInfo->bookTable()).arg(i));
                    if(m_simpleQuery->next()) {
                        m_bookInfo->setFirstPage(m_simpleQuery->value(0).toInt(), i);
                        m_bookInfo->setLastPage(m_simpleQuery->value(1).toInt(), i);
                    }
                }
            }
        }

        m_indexDB->updateBookMeta(m_bookInfo, false);
    }
}

int SimpleDBHandler::maxPartNum()
{
    QSqlQuery query(m_bookDB);
    query.exec(QString("SELECT partNum FROM %1 ORDER BY id DESC")
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

