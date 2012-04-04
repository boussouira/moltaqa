#include "databasemanager.h"
#include "utils.h"
#include <qsqlerror.h>

DatabaseManager::DatabaseManager(QObject *parent) :
    ListManager(parent)
{
}

DatabaseManager::~DatabaseManager()
{
    if(m_db.isOpen())
        m_remover.connectionName = m_db.connectionName();
}

void DatabaseManager::setDatabasePath(const QString &path)
{
    m_path = path;
}

void DatabaseManager::openDatabase()
{
    ML_ASSERT2(!m_path.isEmpty(), "DatabaseManager: Database path is empty");

    QString conn = "DatabaseManager." + QFileInfo(m_path).baseName();
    while(m_db.contains(conn))
        conn.append('_');

    m_db = QSqlDatabase::addDatabase("QSQLITE", conn);
    m_db.setDatabaseName(m_path);

    ML_OPEN_DB(m_db);

    m_query = QSqlQuery(m_db);
}

void DatabaseManager::transaction()
{
    ML_ASSERT2(m_db.transaction(),
               "DatabaseManager: Error on transaction:" << m_db.lastError().text());
}

void DatabaseManager::commit()
{
    ML_ASSERT2(m_db.commit(),
               "DatabaseManager: Error on commit:" << m_db.lastError().text());
}
