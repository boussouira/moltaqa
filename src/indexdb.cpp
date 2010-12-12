#include "indexdb.h"
#include "connectioninfo.h"
#include "bookinfo.h"

IndexDB::IndexDB()
{
}

IndexDB::IndexDB(ConnectionInfo *info)
{
    m_connInfo = info;
}

IndexDB::~IndexDB()
{
    delete m_connInfo;
}

void IndexDB::setConnectionInfo(ConnectionInfo *info)
{
    m_connInfo = info;
}

ConnectionInfo *IndexDB::connectionInfo()
{
    return m_connInfo;
}
