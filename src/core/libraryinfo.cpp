#include "libraryinfo.h"
#include "utils.h"
#include "xmlutils.h"
#include "ziputils.h"

#include <qdir.h>
#include <qdom.h>
#include <qfile.h>
#include <qtextstream.h>

LibraryInfo::LibraryInfo()
{
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

void LibraryInfo::setPath(const QString &path)
{
    m_path = path;
}

void LibraryInfo::setBooksDir(const QString &dir)
{
    m_booksDir = dir;
}

void LibraryInfo::setName(const QString &name)
{
    m_name = name;
}

void LibraryInfo::load(const QString &path)
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

    if(root.attribute("revision").toInt() != 1) {
        throw BookException(tr("اصدار المكتبة غير متوافق مع هذا الاصدار من البرنامج"));
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

    QString dirsName = "abcdefghijklmnpqrstuvwxyz0123456789";
    for(int i=0; i<dirsName.size(); i++) {
        QString dirName = QString("%1").arg(dirsName[i]);

        if(!booksDir.exists(dirName))
            booksDir.mkdir(dirName);
    }

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

void LibraryInfo::checkDataFiles(const QString &dataDirPath)
{
    QDir dataDir(dataDirPath);

    QStringList files;
    QStringList docName;

    files << "bookslist.xml" << "taffesirlist.xml" << "favourites.xml" << "usage.xml";
    docName << "books-list" << "taffesir-list" << "favourites" << "statistics";

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
    dbs << "books.db" << "search.db" << "library.db";

    foreach (QString db, dbs) {
        if(!dataDir.exists(db)) {
            dataDir.cdUp();
            Utils::Library::createDatabases(dataDir.absolutePath());
            break;
        }
    }

    dataDir.setPath(dataDirPath);

    QStringList zipFiles;
    zipFiles << "rowat" << "authors";
    foreach(QString file, zipFiles) {
        QString dbFile = file + ".db";

        if(!dataDir.exists(dbFile)) {
            QDir appDataDir(App::dataDir());

            QString zipFile = file + ".zip";
            if(Utils::Zip::unzip(appDataDir.absoluteFilePath(zipFile), dataDir.absolutePath())) {
                qDebug() << "checkDataFiles: unzip"
                         << appDataDir.absoluteFilePath(zipFile)
                         << "to"
                         << dataDir.absolutePath();
            } else {
                qWarning() << "LibraryInfo::checkDataFiles can't unzip"
                           << appDataDir.absoluteFilePath(zipFile)
                           << "to"
                           << dataDir.absolutePath();
            }
        }
    }
}

QString LibraryInfo::bookPath(QString fileName)
{
    if(fileName.contains(QLatin1String("${"))) {
        return fileName.replace("${DATA_DIR}", App::dataDir());
    } else {
        QDir dir(m_booksDir);
        QString bookDir = fileName.split('_').last().at(0);

        dir.cd(bookDir);

        return dir.filePath(fileName);
    }
}

QString LibraryInfo::bookPath(LibraryBook::Ptr book)
{
    return bookPath(book->fileName);
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

