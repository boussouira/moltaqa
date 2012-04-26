#include "databasemanager.h"
#include "utils.h"
#include <qsqlerror.h>

DatabaseManager::DatabaseManager(QObject *parent) :
    ListManager(parent)
{
}

DatabaseManager::~DatabaseManager()
{
    m_remover.removeDatabase(m_db);
}

void DatabaseManager::setDatabasePath(const QString &path)
{
    m_path = path;
}

void DatabaseManager::openDatabase()
{
    ml_return_on_fail2(!m_path.isEmpty(), "DatabaseManager: Database path is empty");

    QString conn = "DatabaseManager." + QFileInfo(m_path).baseName();
    while(m_db.contains(conn))
        conn.append('_');

    m_db = QSqlDatabase::addDatabase("QSQLITE", conn);
    m_db.setDatabaseName(m_path);

    ml_open_db(m_db);

    m_query = QSqlQuery(m_db);
}

void DatabaseManager::transaction()
{
    ml_return_on_fail2(m_db.transaction(),
               "DatabaseManager: Error on transaction:" << m_db.lastError().text());
}

void DatabaseManager::commit()
{
    ml_return_on_fail2(m_db.commit(),
               "DatabaseManager: Error on commit:" << m_db.lastError().text());
}
