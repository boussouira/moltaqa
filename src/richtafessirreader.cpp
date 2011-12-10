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
#include "mainwindow.h"
#include "bookreaderhelper.h"

#include <qsqlquery.h>
#include <qstringlist.h>
#include <qsqlerror.h>
#include <qdebug.h>
#include <qdatetime.h>

RichTafessirReader::RichTafessirReader(QObject *parent, bool showQuran) : RichBookReader(parent)
{
    m_formatter = new TafessirTextFormat();
    m_textFormat = m_formatter;
    m_quranInfo = 0;
    m_showQuran = showQuran;
}

RichTafessirReader::~RichTafessirReader()
{
    if(m_quranDB.isOpen()) {
        QString conn = m_quranDB.connectionName();
        m_quranDB.close();
        m_quranDB = QSqlDatabase();
        QSqlDatabase::removeDatabase(conn);
    }
}

void RichTafessirReader::connected()
{
    m_quranInfo = m_libraryManager->getQuranBook();
    if(m_quranInfo && m_showQuran)
        openQuranBook();

    RichBookReader::connected();
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

    TafessirQuery tafessirQuery(m_bookDB, m_bookInfo);
    if(id >= m_currentPage->pageID)
        tafessirQuery.nextPage(id);
    else
        tafessirQuery.prevPage(id);

    QString text;
    if(tafessirQuery.next()) {
        m_currentPage->pageID = tafessirQuery.value(0).toInt();
        m_currentPage->part = tafessirQuery.value(2).toInt();
        m_currentPage->page = tafessirQuery.value(3).toInt();
        m_currentPage->aya =  tafessirQuery.value(4).toInt();
        m_currentPage->sora =  tafessirQuery.value(5).toInt();

        text = QString::fromUtf8(qUncompress(tafessirQuery.value(1).toByteArray()));
    }

    m_currentPage->titleID = getPageTitleID(m_currentPage->pageID);

    // TODO: don't show quran text when browsing tafessir book directly?
    if(m_quranInfo && m_showQuran) {
        readQuranText(m_currentPage->sora,
                      m_currentPage->aya,
                      tafessirQuery.getAyatCount(m_currentPage->sora, m_currentPage->aya));
    }

    if(m_query && m_highlightPageID == m_currentPage->pageID)
        m_formatter->insertText(Utils::highlightText(text, m_query, false));
    else
        m_formatter->insertText(text);

    m_formatter->done();

    emit textChanged();
}

void RichTafessirReader::goToPage(int page, int part)
{
    TafessirQuery tafessirQuery(m_bookDB, m_bookInfo);

    tafessirQuery.page(page, part);
    if(tafessirQuery.next())
        goToPage(tafessirQuery.value(0).toInt());
}

BookIndexModel * RichTafessirReader::indexModel()
{
    m_indexModel= new BookIndexModel();
    BookIndexNode *rootNode = new BookIndexNode();

    childTitles(rootNode, 0);

    m_indexModel->setRootNode(rootNode);

    return m_indexModel;
}

BookIndexModel *RichTafessirReader::topIndexModel()
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

void RichTafessirReader::childTitles(BookIndexNode *parentNode, int tid)
{
    QSqlQuery query(m_bookDB);
    query.exec(QString("SELECT id, parentID, pageID, title FROM bookIndex "
                       "WHERE parentID = %1 ORDER BY id").arg(tid));
    while(!m_stopModelLoad && query.next()) {
        BookIndexNode *catNode = new BookIndexNode(query.value(3).toString(),
                                                   query.value(2).toInt());
        childTitles(catNode, query.value(0).toInt());
        parentNode->appendChild(catNode);
    }
}

void RichTafessirReader::openQuranBook()
{
    QString connName = QString("quran_tafessir_%1").arg(m_bookInfo->bookID);

    while(QSqlDatabase::contains(connName)) {
        connName.append("_");
    }

    m_quranDB = QSqlDatabase::addDatabase("QSQLITE", connName);
    m_quranDB.setDatabaseName(m_quranInfo->bookPath);

    if (!m_quranDB.open())
        throw BookException(tr("لم يمكن فتح قاعدة البيانات"), m_quranInfo->bookPath);
}

void RichTafessirReader::readQuranText(int sora, int aya, int count)
{
    if(count>0) {
        QSqlQuery quranQuery(m_quranDB);
        QuranSora *soraInfo = MW->readerHelper()->getQuranSora(sora);

        m_formatter->beginQuran(soraInfo->name, aya, aya+count-1);

        quranQuery.exec(QString("SELECT quranText.ayaText, quranText.ayaNumber, "
                                   "quranText.soraNumber "
                                   "FROM quranText "
                                   "WHERE quranText.ayaNumber >= %1 AND quranText.soraNumber = %2 "
                                   "ORDER BY quranText.id LIMIT %3").arg(aya).arg(sora).arg(count));
        while(quranQuery.next()) {
            m_formatter->insertAyaText(quranQuery.value(0).toString(),
                                       quranQuery.value(1).toInt(),
                                       quranQuery.value(2).toInt());
        }

        m_formatter->endQuran();
    }
}

void RichTafessirReader::goToSora(int sora, int aya)
{
    TafessirQuery tafessirQuery(m_bookDB, m_bookInfo);

    int pageID = tafessirQuery.getPageID(sora, aya);

    if(pageID > 0)
        goToPage(pageID);
}

void RichTafessirReader::goToHaddit(int hadditNum)
{
    TafessirQuery tafessirQuery(m_bookDB, m_bookInfo);

    int pageID = tafessirQuery.getHaddithPage(hadditNum);
    if(pageID > 0)
        goToPage(pageID);
}
