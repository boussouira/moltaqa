#include "sqliteconnection.h"

SqliteConnection::SqliteConnection()
{
    m_type = SQLITE;
    m_driverName = "QSQLITE";
}
