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

void AbstractDBHandler::openQuranDB(QString pQuranDBPath)
{
    QString bookPath = pQuranDBPath.isEmpty() ? m_bookInfo->bookPath() : pQuranDBPath;
    if(QSqlDatabase::contains("QuranTextDB")) {
        m_bookDB = QSqlDatabase::database("QuranTextDB");
    } else {
        m_bookDB = QSqlDatabase::addDatabase("QSQLITE", "QuranTextDB");
        m_bookDB.setDatabaseName(bookPath);
    }

    if (!m_bookDB.open()) {
        qDebug() << "Cannot open database.";
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
