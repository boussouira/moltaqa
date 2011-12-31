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

    if(m_currentPage)
        delete m_currentPage;
}

void AbstractBookReader::openBook(bool fastOpen)
{
    Q_CHECK_PTR(m_bookInfo);

    if(!QFile::exists(m_bookInfo->bookPath)) {
        throw BookException(tr("لم يتم العثور على ملف الكتاب"), bookInfo()->bookPath);
    }

    m_zipFile.setFileName(m_bookInfo->bookPath);
    m_zip.setIoDevice(&m_zipFile);

    if(!m_zip.open(QuaZip::mdUnzip)) {
        throw BookException(tr("لا يمكن فتح ملف الكتاب"), bookInfo()->bookPath, m_zip.getZipError());
    }

    connected();
    getBookInfo();
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

QDomElement AbstractBookReader::getPage(int pid)
{
    QDomElement e = m_rootElement.firstChildElement();

    while(!e.isNull()) {
        int pageID = e.attribute("id").toInt();

        if(pageID == pid) {
            return e;
        }

        e = e.nextSiblingElement();
    }

    return QDomElement();
}

QDomElement AbstractBookReader::getPageId(int page, int part)
{
    QString pageNum = QString::number(page);
    QString partNum = QString::number(part);

    QDomElement e = m_rootElement.firstChildElement();
    while(!e.isNull()) {

        if(pageNum == e.attribute("page") && partNum == e.attribute("part")) {
            return e;
        }

        e = e.nextSiblingElement();
    }

    return QDomElement();
}

QDomElement AbstractBookReader::getPageId(int haddit)
{
    QString hadditNum = QString::number(haddit);

    QDomElement e = m_rootElement.firstChildElement();
    while(!e.isNull()) {

        if(hadditNum == e.attribute("haddit")) {
            return e;
        }

        e = e.nextSiblingElement();
    }

    return QDomElement();
}

QDomElement AbstractBookReader::getQuranPageId(int sora, int aya)
{
    QString soraNum = QString::number(sora);
    QString ayaNum = QString::number(aya);

    QDomElement e = m_rootElement.firstChildElement();
    while(!e.isNull()) {

        if(soraNum == e.attribute("sora") && ayaNum == e.attribute("aya")) {
            return e;
        }

        e = e.nextSiblingElement();
    }

    return QDomElement();
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

void AbstractBookReader::goToPage(int pid)
{
    QDomElement e = getPage(pid);

    if(!e.isNull())
        setCurrentPage(e);
}

void AbstractBookReader::goToPage(int page, int part)
{
    QDomElement e = getPageId(page, part);

    if(!e.isNull())
        setCurrentPage(e);
}

void AbstractBookReader::goToSora(int sora, int aya)
{
    QDomElement e = getQuranPageId(sora, aya);

    if(!e.isNull())
        setCurrentPage(e);
}

void AbstractBookReader::goToHaddit(int hadditNum)
{
    QDomElement e = getPageId(hadditNum);

    if(!e.isNull())
        setCurrentPage(e);
}

void AbstractBookReader::firstPage()
{
    QDomElement e = m_rootElement.firstChildElement();

    if(!e.isNull())
        setCurrentPage(e);
}

void AbstractBookReader::lastPage()
{
    QDomElement e = m_rootElement.lastChildElement();

    if(!e.isNull())
        setCurrentPage(e);
}

void AbstractBookReader::nextPage()
{
    QDomElement e = m_currentElement.nextSiblingElement();

    if(!e.isNull())
        setCurrentPage(e);
}

void AbstractBookReader::prevPage()
{
    QDomElement e = m_currentElement.previousSiblingElement();

    if(!e.isNull())
        setCurrentPage(e);
}

bool AbstractBookReader::hasNext()
{
    return !m_currentElement.nextSibling().isNull();
}

bool AbstractBookReader::hasPrev()
{
    return !m_currentElement.previousSibling().isNull();
}

BookPage *AbstractBookReader::getBookPage(LibraryBook *book, int pageID)
{
    BookPage *page = 0;

    if(!book) {
        qWarning("getBookPage: No book with given id");
        return page;
    }

    if(!QFile::exists(book->bookPath)) {
        qWarning() << "File doesn't exists:" << book->bookPath;
        return 0;
    }

    QFile zipFile(book->bookPath);
    QuaZip zip(&zipFile);

    if(!zip.open(QuaZip::mdUnzip)) {
        qWarning() << "Can't open zip file:" << book->bookPath << "- Error:" << zip.getZipError();
    }

    if(book->isNormal())
        page = getSimpleBookPage(book, &zip, pageID);
    else if(book->isTafessir())
        page = getTafessirPage(book, &zip, pageID);
    else if(book->isQuran())
        page = getQuranPage(book, &zip, pageID);
    else
        qWarning("getBookPage: Unknow book type");

    zip.close();

    return page;
}

BookPage *AbstractBookReader::getSimpleBookPage(LibraryBook *book, QuaZip *zip, int pageID)
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

BookPage *AbstractBookReader::getTafessirPage(LibraryBook *book, QuaZip *zip, int pageID)
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

BookPage *AbstractBookReader::getQuranPage(LibraryBook *book, QuaZip *zip, int pageID)
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

QString AbstractBookReader::getFileContent(QuaZip *zip, QString fileName)
{
    QuaZipFile file(zip);

    if(zip->setCurrentFile(fileName)) {
        if(file.open(QIODevice::ReadOnly)) {
            return QString::fromUtf8(file.readAll());
        } else {
            qWarning("testRead(): file.open(): %d", file.getZipError());
        }
    } else {
        qWarning("setCurrentFile: %d", zip->getZipError());
    }

    return QString();
}

void AbstractBookReader::getBookInfo()
{
   m_pagesMetaFile.setZip(&m_zip);

   if(m_zip.setCurrentFile("pages.xml")) {
       if(!m_pagesMetaFile.open(QIODevice::ReadOnly)) {
           qWarning("testRead(): file.open(): %d", m_pagesMetaFile.getZipError());
           return;
       }
   }

   if(!m_bookDoc.setContent(&m_pagesMetaFile)) {
       qDebug("Error");
       m_pagesMetaFile.close();
       return;
   }

   m_rootElement = m_bookDoc.documentElement();
}
