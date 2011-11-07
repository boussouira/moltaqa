#include "abstractbookreader.h"
#include "libraryinfo.h"
#include "librarybook.h"
#include "bookindexmodel.h"
#include "bookexception.h"
#include "textformatter.h"
#include "utils.h"
#include "mainwindow.h"
#include "bookreaderhelper.h"

#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsqlerror.h>
#include <qstringlistmodel.h>
#include <QTime>
#include <QDebug>

AbstractBookReader::AbstractBookReader(QObject *parent) : QObject(parent)
{
    m_indexModel = 0;
    m_currentPage = new BookPage();
    m_bookInfo = 0;
}

AbstractBookReader::~AbstractBookReader()
{
    if(m_indexModel)
        m_remover.removeModel = m_bookInfo->bookID;

    if(m_bookInfo)
        delete m_bookInfo;

    if(m_currentPage)
        delete m_currentPage;
}

void AbstractBookReader::openBook(bool fastOpen)
{
    Q_ASSERT(m_bookInfo);

    m_connectionName = QString("book_i%1_").arg(m_bookInfo->bookID);
    while(QSqlDatabase::contains(m_connectionName))
        m_connectionName.append("_");

    m_bookDB = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
    m_bookDB.setDatabaseName(m_bookInfo->bookPath);


    if (!m_bookDB.open())
        throw BookException(tr("لم يمكن فتح قاعدة البيانات")+ ": " +m_bookInfo->bookPath,
                            m_bookDB.lastError().text());

    m_bookQuery = QSqlQuery(m_bookDB);

    m_remover.connectionName = m_connectionName;

    if(!fastOpen) {
        connected();
        getBookInfo();
    }
}

void AbstractBookReader::setBookInfo(LibraryBook *bi)
{
    m_bookInfo = bi;
}

void AbstractBookReader::nextAya()
{
}

void AbstractBookReader::prevAya()
{
}

void AbstractBookReader::connected()
{
}

void AbstractBookReader::setLibraryManager(LibraryManager *db)
{
    m_libraryManager = db;
}

void AbstractBookReader::setBookIndexModel(BookIndexModel *model)
{
    m_indexModel = model;
}

LibraryManager* AbstractBookReader::libraryManager()
{
    return m_libraryManager;
}

BookPage * AbstractBookReader::page()
{
    return m_currentPage;
}

void AbstractBookReader::goToSora(int sora, int aya)
{
    Q_UNUSED(sora);
    Q_UNUSED(aya);
}

void AbstractBookReader::goToHaddit(int hadditNum)
{
    Q_UNUSED(hadditNum);
}

void AbstractBookReader::nextPage()
{
    if(hasNext())
        goToPage(m_currentPage->pageID+1);
}

void AbstractBookReader::prevPage()
{
    if(hasPrev())
        goToPage(m_currentPage->pageID-1);
}

bool AbstractBookReader::hasNext()
{
    return (m_currentPage->pageID < m_bookInfo->lastID);
}

bool AbstractBookReader::hasPrev()
{
    return (m_currentPage->pageID > m_bookInfo->firstID);
}

BookPage *AbstractBookReader::getBookPage(LibraryBook *book, int pageID)
{
    BookPage *page = 0;

    if(!book) {
        qWarning("getBookPage: No book with given id");
        return page;
    }

    if(book->isNormal())
        page = getSimpleBookPage(book, pageID);
    else if(book->isTafessir())
        page = getTafessirPage(book, pageID);
    else if(book->isQuran())
        page = getQuranPage(book, pageID);
    else
        qWarning("getBookPage: Unknow book type");

    return page;
}

BookPage *AbstractBookReader::getSimpleBookPage(LibraryBook *book, int pageID)
{
    BookPage *page = 0;
    QString connName = QString("getSimpleBookPage_b%1_p%2").arg(book->bookID).arg(pageID);
    Utils::DatabaseRemover remover;
    QSqlDatabase bookDB;

    while(QSqlDatabase::contains(connName))
        connName.append('_');

    bookDB = QSqlDatabase::addDatabase("QSQLITE", connName);
    bookDB.setDatabaseName(book->bookPath);

    if (!bookDB.open()) {
        LOG_DB_ERROR(bookDB);
        return 0;
    }

    QSqlQuery bookQuery(bookDB);

    bookQuery.prepare(QString("SELECT %1.id, %1.pageText, %1.partNum, %1.pageNum, %2.title "
                              "FROM %1 "
                              "LEFT JOIN %2 "
                              "ON %2.pageID <= %1.id "
                              "WHERE %1.id = ? "
                              "ORDER BY %2.pageID DESC "
                              "LIMIT 1").arg(book->textTable, book->indexTable));
    bookQuery.bindValue(0, pageID);
    if(bookQuery.exec()) {
        if(bookQuery.first()){
            page = new BookPage();
            page->pageID = bookQuery.value(0).toInt();
            page->part = bookQuery.value(2).toInt();
            page->page = bookQuery.value(3).toInt();
            page->text = QString::fromUtf8(qUncompress(bookQuery.value(1).toByteArray()));
            page->title = bookQuery.value(4).toString();
        } else {
            qWarning("No result found for id %d book %d", pageID, book->bookID);
        }
    } else {
        LOG_SQL_ERROR(bookQuery);
    }

    remover.connectionName = connName;

    return page;
}

BookPage *AbstractBookReader::getTafessirPage(LibraryBook *book, int pageID)
{
    BookPage *page = 0;
    QString connName = QString("getTafessirPage_b%1_p%2").arg(book->bookID).arg(pageID);
    Utils::DatabaseRemover remover;
    QSqlDatabase bookDB;

    while(QSqlDatabase::contains(connName))
        connName.append('_');

    bookDB = QSqlDatabase::addDatabase("QSQLITE", connName);
    bookDB.setDatabaseName(book->bookPath);

    if (!bookDB.open()) {
        LOG_DB_ERROR(bookDB);
        return 0;
    }

    QSqlQuery bookQuery(bookDB);

    bookQuery.prepare(QString("SELECT bookPages.id, bookPages.pageText, "
                              "bookPages.partNum, bookPages.pageNum, bookIndex.title, "
                              "tafessirMeta.aya_number , tafessirMeta.sora_number "
                              "FROM bookPages "
                              "LEFT JOIN bookIndex "
                              "ON bookIndex.pageID <= bookPages.id "
                              "LEFT JOIN tafessirMeta "
                              "ON tafessirMeta.page_id = bookPages.id "
                              "WHERE bookPages.id = ? "
                              "ORDER BY bookIndex.pageID DESC "
                              "LIMIT 1"));

    bookQuery.bindValue(0, pageID);

    if(bookQuery.exec()) {
        if(bookQuery.first()){
            page = new BookPage();
            page->pageID = bookQuery.value(0).toInt();
            page->part = bookQuery.value(2).toInt();
            page->page = bookQuery.value(3).toInt();
            page->sora = bookQuery.value(6).toInt();
            page->aya = bookQuery.value(5).toInt();
            page->text = QString::fromUtf8(qUncompress(bookQuery.value(1).toByteArray()));
            page->title = bookQuery.value(4).toString();
            if(page->title.size() < 9) {
                QuranSora *quranSora = MW->readerHelper()->getQuranSora(page->sora);
                if(quranSora)
                    page->title = tr("تفسير سورة %1، الاية %2").arg(quranSora->name).arg(page->aya);
            }

        } else {
            qWarning("No result found for id %d book %d", pageID, book->bookID);
        }
    } else {
        LOG_SQL_ERROR(bookQuery);
    }

    remover.connectionName = connName;

    return page;
}

BookPage *AbstractBookReader::getQuranPage(LibraryBook *book, int pageID)
{
    BookPage *page = 0;
    QString connName = QString("getQuranPage_b%1_p%2").arg(book->bookID).arg(pageID);
    Utils::DatabaseRemover remover;
    QSqlDatabase bookDB;

    while(QSqlDatabase::contains(connName))
        connName.append('_');

    bookDB = QSqlDatabase::addDatabase("QSQLITE", connName);
    bookDB.setDatabaseName(book->bookPath);

    if (!bookDB.open()) {
        LOG_DB_ERROR(bookDB);
        return 0;
    }

    QSqlQuery bookQuery(bookDB);

    bookQuery.prepare("SELECT quranText.id, quranText.ayaText, quranText.ayaNumber, "
                      "quranText.pageNumber, quranText.soraNumber, quranSowar.SoraName "
                      "FROM quranText "
                      "LEFT JOIN quranSowar "
                      "ON quranSowar.id = quranText.soraNumber "
                      "WHERE quranText.id = ?");

    bookQuery.bindValue(0, pageID);

    if(bookQuery.exec()) {
        if(bookQuery.first()){
            page = new BookPage();
            page->pageID = bookQuery.value(0).toInt();
            page->text = bookQuery.value(1).toString();
            page->page = bookQuery.value(3).toInt();
            page->aya = bookQuery.value(2).toInt();
            page->sora = bookQuery.value(4).toInt();
            page->title = tr("سورة %1، الاية %2").arg(bookQuery.value(5).toString()).arg(page->aya);
            page->part = 1;
        } else {
            qWarning("No result found for id %d book %d", pageID, book->bookID);
        }
    } else {
        LOG_SQL_ERROR(bookQuery);
    }

    remover.connectionName = connName;

    return page;
}

void AbstractBookReader::getBookInfo()
{
    if(!m_bookInfo->haveInfo()) {
        m_bookQuery.exec(QString("SELECT MAX(partNum), MIN(id), MAX(id) from %1 ").arg(m_bookInfo->textTable));
        if(m_bookQuery.next()) {
            bool ok;
            int parts = m_bookQuery.value(0).toInt(&ok);
            if(!ok)
                qWarning("Can't get parts count");

            m_bookInfo->partsCount = parts;
            m_bookInfo->firstID = m_bookQuery.value(1).toInt();
            m_bookInfo->lastID = m_bookQuery.value(2).toInt();
        }

        m_libraryManager->updateBookMeta(m_bookInfo, false);
    }
}
