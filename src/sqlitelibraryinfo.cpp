#include "sqlitelibraryinfo.h"

SqliteLibraryInfo::SqliteLibraryInfo(QString booksPath) : LibraryInfo(booksPath)
{
    m_type = SQLITE;
    m_driverName = "QSQLITE";
}
