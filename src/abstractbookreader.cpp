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

    m_bookDB = QSqlDatabase();
    QSqlDatabase::removeDatabase(m_connectionName);
}

void AbstractBookReader::openBookDB()
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

    connected();
    getBookInfo();
}

void AbstractBookReader::setBookInfo(LibraryBook *bi)
{
    m_bookInfo = bi;
}

void AbstractBookReader::setConnctionInfo(LibraryInfo *info)
{
    m_connetionInfo = info;
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
