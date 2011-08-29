#include "libraryinfo.h"
#include "bookexception.h"
#include <QFile>
#include <QtXml>

LibraryInfo::LibraryInfo(QString booksPath)
{
    loafInfo(booksPath);
}

QString LibraryInfo::name()
{
    return m_name;
}

QString LibraryInfo::path()
{
    return m_path;
}

QString LibraryInfo::booksDir()
{
    return m_booksDir;
}

void LibraryInfo::setPath(QString path)
{
    m_path = path;
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
    QDomElement tempsDirElement = root.firstChildElement("temps-dir");

    m_name = nameElement.firstChild().nodeValue();
    if(!pathElement.isNull())
        m_path = pathElement.firstChild().nodeValue();
    else
        m_path = path;

    if(!booksDirElement.isNull())
        m_booksDir = booksDirElement.firstChild().nodeValue();
    else
        m_booksDir = "books";

    if(!tempsDirElement.isNull())
        m_tempsDir = tempsDirElement.firstChild().nodeValue();
    else
        m_tempsDir = "temp";

    QDir booksDir(m_path);
    if(!booksDir.exists(m_booksDir))
        booksDir.mkdir(m_booksDir);

    booksDir.cd(m_booksDir);
    m_booksDir = booksDir.absolutePath();

    QDir tempDir(m_path);
    if(!tempDir.exists(m_tempsDir))
        tempDir.mkdir(m_tempsDir);

    tempDir.cd(m_tempsDir);
    m_tempsDir = tempDir.absolutePath();
}

QString LibraryInfo::booksIndexPath()
{
    QDir dir(m_path);
    return dir.filePath("books_index.db");
}

QString LibraryInfo::bookPath(QString bookName)
{
    QDir dir(m_path);
    dir.cd(m_booksDir);

    return dir.filePath(bookName);
}

QString LibraryInfo::tempDir()
{
    return m_tempsDir;
}

