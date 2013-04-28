#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "listmanager.h"
#include "sqlutils.h"

#include <qsqldatabase.h>
#include <qsqlquery.h>

class QStandardItemModel;

class DatabaseManager : public ListManager
{
    Q_OBJECT
public:
    DatabaseManager(QObject *parent = 0);
    ~DatabaseManager();

    void setDatabasePath(const QString &path);
    void openDatabase();
    
    bool transaction();
    bool commit();

protected:
    QString m_path; ///< SQLite database path
    DatabaseRemover m_remover;
    QSqlDatabase m_db;
    QSqlQuery m_query;
};

#endif // DATABASEMANAGER_H
