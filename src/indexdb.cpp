#include "indexdb.h"
#include "libraryinfo.h"
#include "bookinfo.h"

IndexDB::IndexDB()
{
}

IndexDB::IndexDB(LibraryInfo *info)
{
    m_libraryInfo = info;
}

IndexDB::~IndexDB()
{
    delete m_libraryInfo;
}

void IndexDB::setConnectionInfo(LibraryInfo *info)
{
    m_libraryInfo = info;
}

LibraryInfo *IndexDB::connectionInfo()
{
    return m_libraryInfo;
}
