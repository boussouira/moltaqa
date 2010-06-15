#include "abstractdbhandler.h"

AbstractDBHandler::AbstractDBHandler()
{
}

void AbstractDBHandler::openQuranDB(QString pQuranDBPath)
{
    if(QSqlDatabase::contains("QuranTextDB")) {
        m_bookDB = QSqlDatabase::database("QuranTextDB");
    } else {
        m_bookDB = QSqlDatabase::addDatabase("QSQLITE", "QuranTextDB");
        m_bookDB.setDatabaseName(pQuranDBPath);
    }

    if (!m_bookDB.open()) {
        qDebug() << "Cannot open database.";
    }
    m_bookQuery = new QSqlQuery(m_bookDB);
}
