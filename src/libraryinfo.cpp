#include "libraryinfo.h"
#include "bookexception.h"
#include "xmlutils.h"
#include "utils.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qdir.h>
#include <QDomDocument>

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

QString LibraryInfo::dataDir()
{
    return m_dataDir;
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
    QDir dir(path);
    QString infoFile = dir.filePath("info.xml");

    if(!QFile::exists(infoFile))
        throw BookException(tr("لم يتم العثور على الملف:"), infoFile);

    QDomDocument doc = Utils::Xml::getDomDocument(infoFile);

    m_name = "Library";
    m_path = dir.absolutePath();
    m_dataDir = "data";
    m_booksDir = "books";
    m_tempsDir = "temp";
    m_indexDir = "index";
    m_indexDataDir = "data";
    m_trackerFile = "tracker.xml";

    QDomElement root = doc.documentElement();
    QDomElement e = root.firstChildElement();
    while(!e.isNull()) {
        if(e.tagName() == "name")
            m_name = e.text();
        else if(e.tagName() == "path")
            m_path = QDir::cleanPath(dir.absoluteFilePath(e.text()));
        else if(e.tagName() == "data-dir")
            m_dataDir = e.text();
        else if(e.tagName() == "books-dir")
            m_booksDir = e.text();
        else if(e.tagName() == "temps-dir")
            m_tempsDir = e.text();
        else if(e.tagName() == "index-dir")
            m_indexDir = e.text();
        else if(e.tagName() == "tracker-file")
            m_trackerFile = e.text();
        else if(e.tagName() == "index-datat-dir")
            m_indexDataDir = e.text();

        e = e.nextSiblingElement();
    }

    QDir dataDir(m_path);
    if(!dataDir.exists(m_dataDir))
        dataDir.mkdir(m_dataDir);

    dataDir.cd(m_dataDir);
    m_dataDir = dataDir.absolutePath();
    checkDataFiles(m_dataDir);

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

    QDir indexDir(m_path);
    if(!indexDir.exists(m_indexDir))
        indexDir.mkdir(m_indexDir);

    indexDir.cd(m_indexDir);
    m_indexDir = indexDir.absolutePath();
    m_trackerFile = indexDir.absoluteFilePath(m_trackerFile);

    if(!indexDir.exists(m_trackerFile)) {
        QFile file(m_trackerFile);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out.setCodec("utf-8");

            out << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
            out << "<task-list>";
            out << "</task-list>";
        }
    }

    if(!indexDir.exists(m_indexDataDir))
        indexDir.mkdir(m_indexDataDir);

    indexDir.cd(m_indexDataDir);

    m_indexDataDir = indexDir.absolutePath();
}

void LibraryInfo::checkDataFiles(QString dataDirPath)
{
    QDir dataDir(dataDirPath);

    QStringList files;
    QStringList docName;

    files << "bookslist.xml" << "taffesirlist.xml" << "favourites.xml";
    docName << "books-list" << "taffesir-list" << "favourites";

    for(int i=0; i<files.size(); i++) {
        if(!dataDir.exists(files.at(i))) {
            qDebug("checkDataFiles: create %s...", qPrintable(files.at(i)));

            QFile file(dataDir.filePath(files.at(i)));
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                out.setCodec("utf-8");

                out << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>" << "\n";
                out << "<" << docName.at(i) << ">";
                out << "</" << docName.at(i) << ">";
            }
        }
    }

    QStringList dbs;
    dbs << "authors.db" << "books.db" << "search.db";

    foreach (QString db, dbs) {
        if(!dataDir.exists(db)) {
            dataDir.cdUp();
            Utils::Library::createDatabases(dataDir.absolutePath());
            break;
        }
    }

    dataDir.setPath(dataDirPath);

    QString rowatDB  = "rowat.db";
    if(!dataDir.exists(rowatDB)) {
        QDir appDataDir(App::dataDir());

        if(QFile::copy(appDataDir.absoluteFilePath(rowatDB), dataDir.absoluteFilePath(rowatDB))) {
            qDebug() << "checkDataFiles: copy"
                     << appDataDir.absoluteFilePath(rowatDB)
                     << "->"
                     << dataDir.absoluteFilePath(rowatDB);
        } else {
            qWarning() << "LibraryInfo::checkDataFiles can't copy"
                       << appDataDir.absoluteFilePath(rowatDB)
                       << "->"
                       << dataDir.absoluteFilePath(rowatDB);
        }
    }
}

QString LibraryInfo::bookPath(QString bookName)
{
    QDir dir(m_booksDir);

    return dir.filePath(bookName);
}

QString LibraryInfo::tempDir()
{
    return m_tempsDir;
}

QString LibraryInfo::indexDir()
{
    return m_indexDir;
}

QString LibraryInfo::trackerFile()
{
    return m_trackerFile;
}

QString LibraryInfo::indexDataDir()
{
    return m_indexDataDir;
}

