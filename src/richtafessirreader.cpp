#include "richtafessirreader.h"
#include "tafessirtextformat.h"
#include "tafessirquery.h"
#include "librarybook.h"
#include "bookindexmodel.h"
#include "bookindexnode.h"
#include "libraryinfo.h"
#include "bookindexmodel.h"
#include "bookindexnode.h"
#include "bookexception.h"
#include "utils.h"

#include <qsqlquery.h>
#include <qstringlist.h>
#include <qsqlerror.h>
#include <qdebug.h>
#include <qdatetime.h>

RichTafessirReader::RichTafessirReader(QObject *parent) : RichBookReader(parent)
{
    m_formatter = new TafessirTextFormat();
    m_textFormat = m_formatter;
    m_tafessirQuery = 0;
    m_quranInfo = 0;
}

RichTafessirReader::~RichTafessirReader()
{
    delete m_formatter;

    if(m_tafessirQuery)
        delete m_tafessirQuery;

    if(m_quranDB.isOpen()) {
        m_quranDB.close();
        delete m_quranQuery;
    }
}

void RichTafessirReader::connected()
{
    m_tafessirQuery = new TafessirQuery(m_bookDB, m_bookInfo);

    m_quranInfo = m_libraryManager->getQuranBook();
    if(m_quranInfo)
        openQuranBook();

    AbstractBookReader::connected();
}

void RichTafessirReader::goToPage(int pid)
{
    m_formatter->start();

    int id;
    if(pid == -1) // First page
        id = m_bookInfo->firstID;
    else if(pid == -2) //Last page
        id = m_bookInfo->lastID;
    else // The given page id
        id = pid;

    if(id >= m_bookInfo->currentPage.pageID)
        m_tafessirQuery->nextPage(id);
    else
        m_tafessirQuery->prevPage(id);

    QString text;
    if(m_tafessirQuery->next()) {
        m_bookInfo->currentPage.pageID = m_tafessirQuery->value(0).toInt();
        m_bookInfo->currentPage.part = m_tafessirQuery->value(2).toInt();
        m_bookInfo->currentPage.page = m_tafessirQuery->value(3).toInt();
        m_bookInfo->currentPage.aya =  m_tafessirQuery->value(4).toInt();
        m_bookInfo->currentPage.sora =  m_tafessirQuery->value(5).toInt();

        text = QString::fromUtf8(qUncompress(m_tafessirQuery->value(1).toByteArray()));
    }

    // TODO: don't show quran text when browsing tafessir book directly?
    if(m_quranInfo) {
        readQuranText(m_bookInfo->currentPage.sora,
                      m_bookInfo->currentPage.aya,
                      m_tafessirQuery->getAyatCount(m_bookInfo->currentPage.sora, m_bookInfo->currentPage.aya));
    }

    m_formatter->insertText(text);

    m_formatter->done();

    emit textChanged();
}

void RichTafessirReader::goToPage(int page, int part)
{
    m_tafessirQuery->page(page, part);
    if(m_tafessirQuery->next())
        goToPage(m_tafessirQuery->value(0).toInt());
}

QAbstractItemModel * RichTafessirReader::indexModel()
{
    BookIndexNode *rootNode = new BookIndexNode();

    childTitles(rootNode, 0);

    m_indexModel->setRootNode(rootNode);

    return m_indexModel;
}

QAbstractItemModel * RichTafessirReader::topIndexModel()
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

void RichTafessirReader::nextPage()
{
    if(hasNext())
        goToPage(m_bookInfo->currentPage.pageID+1);
}

void RichTafessirReader::prevPage()
{
    if(hasPrev())
        goToPage(m_bookInfo->currentPage.pageID-1);
}

bool RichTafessirReader::hasNext()
{
    return (m_bookInfo->currentPage.pageID < m_bookInfo->lastID);
}

bool RichTafessirReader::hasPrev()
{
    return (m_bookInfo->currentPage.pageID > m_bookInfo->firstID);
}

void RichTafessirReader::getBookInfo()
{
    m_bookInfo->textTable = "bookPages";
    m_bookInfo->indexTable = "bookIndex";

    if(!m_bookInfo->haveInfo()) {
        m_tafessirQuery->exec(QString("SELECT MAX(partNum), MIN(id), MAX(id) from %1 ").arg(m_bookInfo->textTable));
        if(m_tafessirQuery->next()) {
            bool ok;
            int parts = m_tafessirQuery->value(0).toInt(&ok);
            if(!ok)
                qWarning("Can't get parts count");

            m_bookInfo->partsCount = parts;
            m_bookInfo->firstID = m_tafessirQuery->value(1).toInt();
            m_bookInfo->lastID = m_tafessirQuery->value(2).toInt();
        }

        m_libraryManager->updateBookMeta(m_bookInfo, false);
    }
}

void RichTafessirReader::childTitles(BookIndexNode *parentNode, int tid)
{
    QSqlQuery query(m_bookDB);
    query.exec(QString("SELECT id, parentID, pageID, title FROM bookIndex "
                       "WHERE parentID = %1 ORDER BY id").arg(tid));
    while(query.next()) {
        BookIndexNode *catNode = new BookIndexNode(query.value(3).toString(),
                                                   query.value(2).toInt());
        childTitles(catNode, query.value(0).toInt());
        parentNode->appendChild(catNode);
    }
}

void RichTafessirReader::openQuranBook()
{
    if(QSqlDatabase::contains(QString("quran_tafessir_%1").arg(m_bookInfo->bookID))) {
        m_quranDB = QSqlDatabase::database(QString("quran_tafessir_%1").arg(m_bookInfo->bookID));
    } else {
        m_quranDB = QSqlDatabase::addDatabase("QSQLITE", QString("quran_tafessir_%1").arg(m_bookInfo->bookID));
        m_quranDB.setDatabaseName(m_quranInfo->bookPath);
    }

    if (!m_quranDB.open())
        throw BookException(tr("لم يمكن فتح قاعدة البيانات"), m_quranInfo->bookPath);

    m_quranQuery = new QSqlQuery(m_quranDB);
}

void RichTafessirReader::readQuranText(int sora, int aya, int count)
{
    if(count>0) {
        m_formatter->beginQuran();

        m_quranQuery->exec(QString("SELECT quranText.id, quranText.ayaText, quranText.ayaNumber, "
                                   "quranText.pageNumber, quranText.soraNumber, quranSowar.SoraName "
                                   "FROM quranText LEFT JOIN quranSowar "
                                   "ON quranSowar.id = quranText.soraNumber "
                                   "WHERE quranText.ayaNumber >= %1 AND quranText.soraNumber = %2 "
                                   "ORDER BY quranText.id LIMIT %3").arg(aya).arg(sora).arg(count));
        while(m_quranQuery->next()) {
            m_formatter->insertAyaText(m_quranQuery->value(1).toString(),
                                       m_quranQuery->value(2).toInt(),
                                       m_quranQuery->value(4).toInt());
        }

        m_formatter->endQuran();
    }
}

void RichTafessirReader::goToSora(int sora, int aya)
{
    int pageID = m_tafessirQuery->getPageID(sora, aya);

    if(pageID > 0)
        goToPage(pageID);
}

void RichTafessirReader::goToHaddit(int hadditNum)
{
    int pageID = m_tafessirQuery->getHaddithPage(hadditNum);

    if(pageID > 0)
        goToPage(pageID);
}
