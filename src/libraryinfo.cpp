#include "libraryinfo.h"
#include "bookexception.h"
#include <QFile>
#include <QtXml>

LibraryInfo::LibraryInfo(QString booksPath)
{
    m_type = UNKNOW;
    loafInfo(booksPath);
}

QString LibraryInfo::name()
{
    return m_name;
}

LibraryInfo::ConnectionType LibraryInfo::type()
{
    return m_type;
}

QString LibraryInfo::path()
{
    return m_path;
}

QString LibraryInfo::username()
{
    return m_username;
}

QString LibraryInfo::password()
{
    return m_password;
}

QString LibraryInfo::server()
{
    return m_server;
}

QString LibraryInfo::driverName()
{
    return m_driverName;
}

QString LibraryInfo::connectionName()
{
    return m_connectionName;
}

QString LibraryInfo::booksDir()
{
    return m_booksDir;
}

void LibraryInfo::setType(ConnectionType type)
{
    m_type = type;
}

void LibraryInfo::setPath(QString path)
{
    m_path = path;
}

void LibraryInfo::setUsername(QString user)
{
    m_username = user;
}

void LibraryInfo::setPassword(QString pass)
{
    m_password = pass;
}

void LibraryInfo::setServer(QString server)
{
    m_server = server;
}

void LibraryInfo::setDriverName(QString name)
{
    m_driverName = name;
}

void LibraryInfo::setConnectionName(QString name)
{
    m_connectionName = name;
}

void LibraryInfo::setBooksDir(QString dir)
{
    m_booksDir = dir;
}

void LibraryInfo::setName(QString name)
{
    m_name = name;
}

void LibraryInfo::loafInfo(QString path)
{
    QDomDocument doc;
    QString infoFile = path + "/info.xml";
    QString errorStr;
    int errorLine;
    int errorColumn;

    if(!QFile::exists(infoFile))
        throw BookException(tr("لم يتم العثور على الملف:"), infoFile);

    QFile file(infoFile);
    if (!file.open(QIODevice::ReadOnly)) {
        throw BookException(tr("حدث خطأ أثناء فتح الملف:"), infoFile);
    }

    if (!doc.setContent(&file, 0, &errorStr, &errorLine, &errorColumn)) {
        throw BookException(tr("Parse error at line %1, column %2: %3").arg(errorLine).arg(errorColumn).arg(errorStr),
                            infoFile);
    }

    QDomElement root = doc.documentElement();
    QDomElement nameElement = root.firstChildElement("name");
    QDomElement pathElement = root.firstChildElement("path");
    QDomElement booksDirElement = root.firstChildElement("books-dir");

    m_name = nameElement.firstChild().nodeValue();
    m_path = pathElement.firstChild().nodeValue();
    m_booksDir = booksDirElement.firstChild().nodeValue();
    m_connectionName = "BooksIndexDB";

    QDir dir(m_path);
    dir.cd(m_booksDir);
    m_booksDir = dir.absolutePath();

    qDebug() << "Name:" << m_name; //DELETE ME!
    qDebug() << "Path:" << m_path; //DELETE ME!
    qDebug() << "Books path:" << m_booksDir; //DELETE ME!
}

QString LibraryInfo::booksIndexPath()
{
    QDir dir(m_path);
    qDebug() << "Index path:" << dir.filePath("books_index.db"); //DELETE ME!
    return dir.filePath("books_index.db");
}

QString LibraryInfo::bookPath(QString bookName)
{
    QDir dir(m_path);
    dir.cd(m_booksDir);
    qDebug() << "Book path:" << dir.filePath(bookName); //DELETE ME!
    return dir.filePath(bookName);
}

