#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "listmanager.h"
#include "qsqldatabase.h"
#include "qsqlquery.h"
#include "sqlutils.h"

class QStandardItemModel;

class DatabaseManager : public ListManager
{
    Q_OBJECT
public:
    DatabaseManager(QObject *parent = 0);
    ~DatabaseManager();

    void setDatabasePath(const QString &path);
    void openDatabase();
    
    void transaction();
    void commit();

protected:
    QString m_path; ///< SQLite database path
    Utils::DatabaseRemover m_remover;
    QSqlDatabase m_db;
    QSqlQuery m_query;
};

#endif // DATABASEMANAGER_H
