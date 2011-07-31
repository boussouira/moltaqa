#ifndef SQLITECONNECTION_H
#define SQLITECONNECTION_H

#include "libraryinfo.h"

class SqliteLibraryInfo : public LibraryInfo
{
public:
    SqliteLibraryInfo(QString booksPath);
};

#endif // SQLITECONNECTION_H
