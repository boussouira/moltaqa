#ifndef CONNECTIONINFO_H
#define CONNECTIONINFO_H

#include <qobject.h>
#include <qcoreapplication.h>

class LibraryInfo
{
    Q_DECLARE_TR_FUNCTIONS(ConnectionInfo)

public:
    /**
        Load information from an info.xml file
        @param booksPath Path where info.xml exsists
    */
    LibraryInfo(QString booksPath);

    QString name();
    QString path();
    QString username();
    QString password();
    QString server();
    QString connectionName();
    /**
      Get full path to books directory
      */
    QString booksDir();

    void setName(QString name);
    void setPath(QString path);
    void setUsername(QString user);
    void setPassword(QString pass);
    void setServer(QString server);
    void setConnectionName(QString name);
    void setBooksDir(QString dir);

    /**
      Get books index databases full path
      */
    QString booksIndexPath();

    /**
      Get book path
      @param bookName The name of book file
      */
    QString bookPath(QString bookName);

protected:
    void loafInfo(QString path);

protected:
    QString m_name;         ///< Library name
    QString m_path;         ///< Library path
    QString m_username;
    QString m_password;
    QString m_server;
    QString m_connectionName;
    QString m_booksDir;     ///< Books dir, it should be relative path to the library path
};

#endif // CONNECTIONINFO_H