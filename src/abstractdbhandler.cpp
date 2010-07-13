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
    if(m_bookDB.isOpen()) {
        delete m_bookQuery;
        m_bookDB.close();
    }
}

void AbstractDBHandler::openBookDB(QString pBookDBPath)
{
    QString bookPath = pBookDBPath.isEmpty() ? m_bookInfo->bookPath() : pBookDBPath;
    if(QSqlDatabase::contains(m_connectionName)) {
        m_bookDB = QSqlDatabase::database(m_connectionName);
    } else {
        m_bookDB = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
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
    m_connectionName = QString("book_i%1").arg(m_bookInfo->bookID());
}

QString AbstractDBHandler::nextUnit()
{
    return QString();
}

QString AbstractDBHandler::prevUnit()
{
    return QString();
}
