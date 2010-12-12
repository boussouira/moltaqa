#include "connectioninfo.h"

ConnectionInfo::ConnectionInfo()
{
    m_type = UNKNOW;
}

ConnectionInfo::ConnectionType ConnectionInfo::type()
{
    return m_type;
}

QString ConnectionInfo::path()
{
    return m_path;
}

QString ConnectionInfo::username()
{
    return m_username;
}

QString ConnectionInfo::password()
{
    return m_password;
}

QString ConnectionInfo::server()
{
    return m_server;
}

QString ConnectionInfo::driverName()
{
    return m_driverName;
}

QString ConnectionInfo::connectionName()
{
    return m_connectionName;
}

QString ConnectionInfo::booksDir()
{
    return m_booksDir;
}

void ConnectionInfo::setType(ConnectionType type)
{
    m_type = type;
}

void ConnectionInfo::setPath(QString path)
{
    m_path = path;
}

void ConnectionInfo::setUsername(QString user)
{
    m_username = user;
}

void ConnectionInfo::setPassword(QString pass)
{
    m_password = pass;
}

void ConnectionInfo::setServer(QString server)
{
    m_server = server;
}

void ConnectionInfo::setDriverName(QString name)
{
    m_driverName = name;
}

void ConnectionInfo::setConnectionName(QString name)
{
    m_connectionName = name;
}

void ConnectionInfo::setBooksDir(QString dir)
{
    m_booksDir = dir;
}
