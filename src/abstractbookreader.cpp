#include "abstractbookreader.h"
#include "libraryinfo.h"
#include "librarybook.h"
#include "bookindexmodel.h"
#include "bookexception.h"
#include "textformatter.h"

#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsqlerror.h>
#include <qstringlistmodel.h>
#include <QTime>
#include <QDebug>

AbstractBookReader::AbstractBookReader(QObject *parent) : QObject(parent)
{
    m_indexModel = new BookIndexModel();
    m_currentPage = new BookPage();
    m_bookInfo = 0;
}

AbstractBookReader::~AbstractBookReader()
{
    delete m_indexModel;

    if(m_bookInfo)
        delete m_bookInfo;

    if(m_currentPage)
        delete m_currentPage;

    m_bookDB = QSqlDatabase();
    QSqlDatabase::removeDatabase(m_connectionName);
}

void AbstractBookReader::openBook(bool fastOpen)
{
    Q_ASSERT(m_bookInfo);

    m_connectionName = QString("book_i%1").arg(m_bookInfo->bookID);
    while(QSqlDatabase::contains(m_connectionName))
        m_connectionName.append("_");

    m_bookDB = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
    m_bookDB.setDatabaseName(m_bookInfo->bookPath);


    if (!m_bookDB.open())
        throw BookException(tr("لم يمكن فتح قاعدة البيانات"), m_bookInfo->bookPath);

    m_bookQuery = QSqlQuery(m_bookDB);

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

void AbstractBookReader::getBookInfo()
{
    m_bookInfo->textTable = "bookPages";
    m_bookInfo->indexTable = "bookIndex";

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
