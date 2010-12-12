#ifndef CONNECTIONINFO_H
#define CONNECTIONINFO_H

#include <qstring.h>

class ConnectionInfo
{
public:
    ConnectionInfo();

    enum ConnectionType {
        UNKNOW,
        SQLITE,
        MYSQL
    };

    ConnectionType type();
    QString path();
    QString username();
    QString password();
    QString server();
    QString driverName();
    QString connectionName();
    QString booksDir();

    void setType(ConnectionType type);
    void setPath(QString path);
    void setUsername(QString user);
    void setPassword(QString pass);
    void setServer(QString server);
    void setDriverName(QString name);
    void setConnectionName(QString name);
    void setBooksDir(QString dir);

protected:
    ConnectionType m_type;
    QString m_path;
    QString m_username;
    QString m_password;
    QString m_server;
    QString m_driverName;
    QString m_connectionName;
    QString m_booksDir;
};

#endif // CONNECTIONINFO_H
