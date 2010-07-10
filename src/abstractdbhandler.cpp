#include "abstractdbhandler.h"

AbstractDBHandler::AbstractDBHandler()
{
    m_indexModel = new BookIndexModel();
    m_bookInfo = new BookInfo();
}

AbstractDBHandler::~AbstractDBHandler()
{
    delete m_indexModel;
    delete m_bookInfo;
    delete m_bookQuery;
    m_bookDB.close();
}

void AbstractDBHandler::openBookDB(QString pBookDBPath)
{
    QString bookPath = pBookDBPath.isEmpty() ? m_bookInfo->bookPath() : pBookDBPath;
    if(QSqlDatabase::contains("QuranTextDB")) {
        m_bookDB = QSqlDatabase::database("QuranTextDB");
    } else {
        m_bookDB = QSqlDatabase::addDatabase("QSQLITE", "QuranTextDB");
        m_bookDB.setDatabaseName(bookPath);
    }

    if (!m_bookDB.open()) {
        qDebug("[%s:%d] Cannot open database.", __FILE__, __LINE__);
    }
    m_bookQuery = new QSqlQuery(m_bookDB);
    getBookInfo();
}

void AbstractDBHandler::setBookInfo(BookInfo *bi)
{
    delete m_bookInfo;
    m_bookInfo = bi;
}

QString AbstractDBHandler::nextUnit()
{
    return QString();
}

QString AbstractDBHandler::prevUnit()
{
    return QString();
}
