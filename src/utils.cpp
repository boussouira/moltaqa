#include "utils.h"
#include "sqlutils.h"

#include <qapplication.h>
#include <qcryptographichash.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qheaderview.h>
#include <qmessagebox.h>
#include <qsettings.h>
#include <qsqldatabase.h>
#include <qsqlerror.h>
#include <qsqlquery.h>
#include <qtoolbar.h>
#include <qtreeview.h>
#include <quuid.h>

static QString appRootPath;
static uint m_randSlat = QDateTime::currentDateTime().toTime_t();

namespace Utils {

namespace Rand {

void srand()
{
    ::srand(++m_randSlat);
}

int number(int smin, int smax)
{
    int rVal = (smin + (rand() % (smax-smin+1)));
    return qBound(smin, rVal, smax);
}

QString fileName(const QString &path, bool fullPath, QString namePrefix, QString ext)
{
    Rand::srand();

    QDir dir(path);
    QString fileName(namePrefix);

    if(ext.size() && !ext.startsWith('.'))
        ext.prepend('.');

    fileName.append(string(6, false));

    while(dir.exists(fileName+ext)) {
        fileName.append(string(1, false));
    }

    if(fullPath)
        return dir.filePath(fileName+ext);
    else
        return fileName+ext;
}

QString newBook(const QString &path)
{
    QDir dir(path);
    QString bookDir = string(1, false);

    if(!dir.exists(bookDir))
        dir.mkdir(bookDir);

    dir.cd(bookDir);

    return fileName(dir.absolutePath(), true, QString("book_")+bookDir);
}

QString uuid()
{
    return QUuid::createUuid()
            .toString()
            .remove('{')
            .remove('}');
}

QString string(int size, bool upperChar, bool lowerChar, bool numbers)
{
    ml_return_val_on_fail2(size, "Rand::string size must be greater than 0", QString());
    ml_return_val_on_fail2(upperChar || lowerChar || numbers,
                           "Rand::string nothing to randomize!", QString());

    Rand::srand();

    QString str;
    QString chars;

    if(lowerChar)
        chars.append("abcdefghijklmnpqrstuvwxyz");

    if(upperChar)
        chars.append("ABCDEFGHIJKLMNPQRSTUVWXYZ");

    if(numbers)
        chars.append("0123456789");

    int smax = chars.size()-1;

    for(int i=0; i<size; i++) {
        str.append(chars.at(number(0, smax)));
    }

    return str;
}
}

namespace Library {

bool isValidLibrary(const QString &path)
{
    QDir dir(path);

    return (dir.exists() && dir.exists("info.xml"));
}

void createDatabases(const QString &path)
{
    QDir dir(path);
    if(!dir.exists("data"))
        dir.mkdir("data");

    dir.cd("data");

    QString booksDbPath = dir.filePath("books.db");
    QString searchDbPath = dir.filePath("search.db");
    QString libraryDbPath = dir.filePath("library.db");

    {
        if(QFile::exists(booksDbPath))
            qDebug("createDatabases: check books database...");
        else
            qDebug("createDatabases: create books database...");

        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "createDB.books");
        db.setDatabaseName(booksDbPath);

        if (!db.open()) {
            ml_warn_db_error(db);
        }

        QSqlQuery query(db);

        QueryBuilder q;
        q.setIgnoreExistingTable(true);

        q.setTableName("books", QueryBuilder::Create);

        q.set("id", "INTEGER PRIMARY KEY NOT NULL");
        q.set("uuid", "TEXT");
        q.set("title", "TEXT");
        q.set("otherTitles", "TEXT");
        q.set("type", "INT");
        q.set("authorID", "INT");
        q.set("author", "TEXT");
        q.set("info", "TEXT");
        q.set("comment", "TEXT");
        q.set("edition", "TEXT");
        q.set("publisher", "TEXT");
        q.set("mohaqeq", "TEXT");
        q.set("bookFlags", "INT");
        q.set("indexFlags", "INT");
        q.set("filename", "TEXT");

        q.exec(query);

        query.prepare("CREATE UNIQUE INDEX uuuid_index ON books (uuid)");
        ml_query_exec(query);

        q.setTableName("books_meta", QueryBuilder::Create);

        q.set("id", "INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL");
        q.set("create_date", "INT");
        q.set("import_date", "INT");
        q.set("update_date", "INT");
        q.set("open_count", "INT");
        q.set("update_count", "INT");
        q.set("result_open_count", "INT");
        q.set("file_checksum", "TEXT");

        q.exec(query);

        q.setTableName("history", QueryBuilder::Create);

        q.set("id", "INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL");
        q.set("book", "INT");
        q.set("page", "INT");
        q.set("open_date", "INT");

        q.exec(query);

        q.setTableName("last_open", QueryBuilder::Create);

        q.set("book", "INTEGER PRIMARY KEY NOT NULL");
        q.set("page", "INT");
        q.set("open_date", "INT");

        q.exec(query);
    }
    {
        if(QFile::exists(searchDbPath))
            qDebug("createDatabases: check search database...");
        else
            qDebug("createDatabases: create search database...");

        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "createDB.search");
        db.setDatabaseName(searchDbPath);

        if (!db.open()) {
            ml_warn_db_error(db);
        }

        QSqlQuery query(db);

        QueryBuilder q;
        q.setTableName("fields", QueryBuilder::Create);
        q.setIgnoreExistingTable(true);

        q.set("id", "INTEGER PRIMARY KEY NOT NULL");
        q.set("name", "TEXT");
        q.set("info", "TEXT");

        q.exec(query);

        q.setTableName("fieldsBooks", QueryBuilder::Create);
        q.setIgnoreExistingTable(true);

        q.set("field", "INTEGER");
        q.set("bookID", "INTEGER");

        q.exec(query);

        q.setTableName("savedSearch", QueryBuilder::Create);
        q.setIgnoreExistingTable(true);

        q.set("id", "INTEGER PRIMARY KEY NOT NULL");
        q.set("query", "TEXT");

        q.exec(query);
    }
    {
        if(QFile::exists(libraryDbPath))
            qDebug("createDatabases: check library database...");
        else
            qDebug("createDatabases: create library database...");

        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "createDB.library");
        db.setDatabaseName(libraryDbPath);

        if (!db.open()) {
            ml_warn_db_error(db);
        }

        QSqlQuery query(db);

        QueryBuilder q;
        q.setTableName("refers", QueryBuilder::Create);
        q.setIgnoreExistingTable(true);

        q.set("id", "INTEGER PRIMARY KEY NOT NULL");
        q.set("name", "TEXT");
        q.set("referText", "TEXT");

        q.exec(query);

        q.setTableName("refers", QueryBuilder::Replace);

        q.set("name", QObject::tr("افتراضي"));
        q.set("referText", _u("قال *المؤلف*: \"*النص*\" (*الكتاب*) (*الجزء*/ *الصفحة*)"));
        q.where("id", 1);

        q.exec(query);
    }

    QSqlDatabase::removeDatabase("createDB.books");
    QSqlDatabase::removeDatabase("createDB.search");
    QSqlDatabase::removeDatabase("createDB.library");
}
}

namespace Widget {

void save(QWidget *w, QString section)
{
    QSettings settings;
    settings.beginGroup("WidgetStat");

    if(!w->isMaximized()) {
        settings.setValue(section + ".pos", w->pos());
        settings.setValue(section + ".size", w->size());
    }

    settings.setValue(section + ".maximized", w->isMaximized());

    settings.endGroup();
}

void restore(QWidget *w, QString section, bool showMaximized)
{
    QSettings settings;
    settings.beginGroup("WidgetStat");

    QRect r(1337,1337,1337,1337);

    QSize size = settings.value(section + ".size", r.size()).toSize();
    if(size != r.size())
        w->resize(size);

    QPoint pos = settings.value(section + ".pos", r.topLeft()).toPoint();
    if(pos != r.topLeft())
        w->move(pos);

    if(settings.value(section + ".maximized", showMaximized).toBool())
        w->showMaximized();

    settings.endGroup();
}

void save(QTreeView *tree, QString section, int columnCount)
{
    QSettings settings;
    settings.beginGroup("TreeViewStat");

    if(columnCount == -1)
        columnCount = tree->header()->count();

    settings.setValue(QString("%1.count").arg(section), columnCount);

    for(int i=0; i<columnCount; i++) {
        settings.setValue(QString("%1.col%2").arg(section).arg(i),
                          tree->columnWidth(i));
    }
}

void restore(QTreeView *tree, QString section, QList<int> defaultWidth)
{
    QSettings settings;
    settings.beginGroup("TreeViewStat");

    int columnCount =settings.value(QString("%1.count").arg(section)).toInt();

    if(!columnCount)
        columnCount = defaultWidth.size();
    else
        columnCount = qMin(columnCount, defaultWidth.size());


    for(int i=0; i<columnCount; i++) {
        int colWidth = settings.value(QString("%1.col%2").arg(section).arg(i),
                                      defaultWidth.at(i)).toInt();

        tree->setColumnWidth(i, colWidth);
    }
}

void save(QToolBar *bar)
{
    ml_return_on_fail2(bar->objectName().size(), "Utils::Widget::save Toolbar name is empty");

    Utils::Settings::set("ToolBars/" + bar->objectName() + ".show", bar->isVisible());
}

void restore(QToolBar *bar)
{
    ml_return_on_fail2(bar->objectName().size(), "Utils::Widget::restore Toolbar name is empty");

    bar->setVisible(Utils::Settings::get("ToolBars/" + bar->objectName() + ".show", true).toBool());
}

void hideHelpButton(QWidget *w)
{
    Qt::WindowFlags flags = w->windowFlags();
    flags |= Qt::WindowContextHelpButtonHint;
    flags ^= Qt::WindowContextHelpButtonHint;

    w->setWindowFlags(flags);
}

int exec(QDialog *dialog, QString section, bool hideHelp)
{
    if(section.size())
        restore(dialog, section);

    if(hideHelp)
        hideHelpButton(dialog);

    int ret = dialog->exec();

    if(section.size())
        save(dialog, section);

    return ret;
}
} // Widget

namespace Files {
bool move(const QString &sourcePath, const QString &destPath)
{
    QString destFile;
    QFileInfo info(destPath);

    if(info.isDir()) {
        QDir dir(destPath);
        destFile = dir.filePath(QFileInfo(sourcePath).fileName());
    } else {
        destFile = destPath;
    }

    ml_return_val_on_fail2(QFile::copy(sourcePath, destFile),
                           "Files::move can't copy" << sourcePath << "to" << destFile,
                           false);

    ml_warn_on_fail(QFile::remove(sourcePath),
                    "Files::move can't delete file" << sourcePath);

    return true;
}

void removeDir(const QString &path)
{
    QFileInfo info(path);
    if(info.isDir()) {
        QDir dir(path);
        foreach(QFileInfo file, dir.entryInfoList(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot)) {
            removeDir(file.filePath());
        }

        dir.rmdir(path);
    } else {
        if(!QFile::remove(path))
            qDebug() << "Utils::Files::removeDir Can't delete:" << path;
    }
}

bool copyData(QIODevice &inFile, QIODevice &outFile)
{
    char buf[4096];
    qint64 len = 0;

    while (!inFile.atEnd()) {
        len = inFile.read(buf, 4096);
        if(len <= 0)
            return false;

        if(outFile.write(buf, len) != len)
            return false;
    }

    return true;
}
bool copyData(QByteArray &inData, QIODevice &outFile)
{
    return outFile.write(inData) != -1;
}

bool copyData(QIODevice &inFile, QByteArray &outData)
{
    char buf[4096];
    int len = 0;

    while (!inFile.atEnd()) {
        len = inFile.read(buf, 4096);
        outData.append(buf, len);

        if(len <= 0)
            return false;
    }

    return true;
}

quint64 directorySize(const QString &path)
{
    QFileInfo info(path);
    quint64 size = 0;

    if(info.isDir()){
        QDir dir(path);
        foreach(QFileInfo fieInfo, dir.entryInfoList(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot)) {
            if(fieInfo.isFile())
                size += fieInfo.size();
            else if(fieInfo.isDir())
                size += directorySize(fieInfo.absoluteFilePath());
        }
    }

    return size;
}

QString formatSize(quint64 size)
{
    QString sizeStr;

    if(size < 1024)
        sizeStr = QObject::tr("%1 بيت").arg(size);
    else if(1024 <= size && size < 1024*1024)
        sizeStr = QObject::tr("%1 كيلو").arg(size/(1024.0), 4, 'g', 2);
    else if( 1024*1024 <= size && size < 1024*1024*1024)
        sizeStr = QObject::tr("%1 ميغا").arg(size/(1024.0*1024.0), 4, 'f', 2);
    else
        sizeStr = QObject::tr("%1 جيجا").arg(size/(1024.0*1024.0*1024.0), 4, 'f', 2);

    return sizeStr;
}

QString cleanFileName(QString fileName, bool removeSpace)
{
    QFileInfo info(fileName);
    QString baseName = info.baseName().remove(QRegExp("[\\?\\*<>\\(\\)\\[\\]\"':]")).simplified();
    QString ext = info.completeSuffix().trimmed();

    if(removeSpace)
         baseName.replace(' ', '_');

    QString path = info.dir().filePath(baseName + '.' + ext);
    if(path.startsWith(QLatin1String("./")))
        path = path.remove(0, 2);

    return path;
}

/**
 * @brief This function ensure the no file exists at the given path
 * if a file already exists at the given path, this function will return
 * a change the file name and return the new path
 * @param path
 * @return path to a new file
 */
QString ensureFileExistsNot(QString path)
{
    if(!QFile::exists(path))
        return path;

    QFileInfo info(path);
    QString baseName = info.baseName();

    QString newPath;

    do {
        if(baseName.contains(QRegExp("_[0-9]+$"))) {
            QRegExp rx("_([0-9]+)$");
            int pos = 0;

            if ((pos = rx.indexIn(baseName, pos)) != -1) {
                int num =rx.cap(1).toInt();
                baseName.replace(QRegExp("_[0-9]+$"), QString("_%1").arg(num+1));
            } else {
                baseName.append("_1");
            }
        } else {
            baseName.append("_1");
        }

        newPath = info.dir().filePath(baseName + '.' + info.completeSuffix());

    } while(QFile::exists(newPath));

    return newPath;
}

QByteArray fileMd5(const QString &path)
{
    QFile file(path);
    ml_return_val_on_fail2(file.open(QFile::ReadOnly),
                           "File::fileMd5 Can't open file" << path <<
                           "Error:" << file.errorString(),
                           QByteArray());

    QCryptographicHash crypto(QCryptographicHash::Md5);
    char buf[8192];
    int len = 0;

    while (!file.atEnd()) {
        len = file.read(buf, 8192);
        crypto.addData(buf, len);

        if(len <= 0)
            return QByteArray();
    }

    return crypto.result().toHex();
}

} // Files

namespace Settings {
QVariant get(const QString &key, const QVariant &defaultValue)
{
    QSettings settings;
    return settings.value(key, defaultValue);
}

void set(const QString &key, const QVariant &defaultValue)
{
    QSettings settings;
    settings.setValue(key, defaultValue);
}

void remove(const QString &key)
{
    QSettings settings;
    settings.remove(key);
}

bool contains(const QString &key)
{
    QSettings settings;
    return settings.contains(key);
}
} // Settings

}

namespace App {
QString name()
{
    return QObject::tr("مكتبة الملتقى");
}

const char *version()
{
    return APP_VERSION_STR;
}

QStringList checkDir(bool showWarnings)
{
    QStringList missingFiles;

    missingFiles << checkFiles(QStringList()
                               << "quran-meta.db"
                               << "rowat.zip"
                               << "authors.zip"
                               << "welcome/welcome.html"
                               << "welcome/welcome.css"
                               << "welcome/images"
                               << "help/help.mlb",
                               dataDir(),
                               showWarnings);

    missingFiles << checkFiles(QStringList()
                               << "jquery.pagination.js"
                               << "jquery.growl.js"
                               << "jquery.tooltip.js"
                               << "jquery.js"
                               << "editor.js"
                               << "reader.js"
                               << "search.js"
                               << "ckeditor"
                               << "scripts.js",
                               jsDir(),
                               showWarnings);

    missingFiles << checkFiles(QStringList()
                               << "qt_ar.qm",
                               localeDir(),
                               showWarnings);

    missingFiles << checkFiles(QStringList()
                               << ML_DEFAULT_STYLE "/config.cfg"
                               << ML_DEFAULT_STYLE "/default.css",
                               stylesDir(),
                               showWarnings);

    return  missingFiles;
}

QStringList checkFiles(QStringList files, QDir dir, bool showWarnings)
{
    QStringList missingFiles;

    foreach(QString fileName, files) {
        if(!dir.exists(fileName)) {
            missingFiles << dir.filePath(fileName);

            if(showWarnings)
                qWarning() << "checkFiles: File doesn't exist" << dir.filePath(fileName);
        }
    }

    return missingFiles;
}

QString appDir()
{
    if(appRootPath.isEmpty()) {
        QDir dir(QApplication::applicationDirPath());
        dir.cdUp();

        appRootPath = dir.absolutePath();
        QStringList missingFiles;

        missingFiles = checkDir(false);
        if(missingFiles.size()) {
            appRootPath = QDir::currentPath();

            missingFiles = checkDir(false);
            if(missingFiles.size()) {
                QStringList missingFileToShow;
                for (int i=0; i<qMin(6, missingFiles.size()); i++)
                    missingFileToShow.append(missingFiles[i]);

                if (missingFiles.size() > 6)
                    missingFileToShow.append("...");

                QMessageBox::critical(0,
                                      App::name(),
                                      QObject::tr("لم يتم العثور على بعض الملفات في مجلد البرنامج""\n"
                                                  "من فضلك قم باعادة تتبيث البرنامج""\n"
                                                  "الملفات الناقصة (%1):""\n""%2")
                                      .arg(missingFiles.size())
                                      .arg(missingFileToShow.join("\n")));

                qFatal("Some files are messing:\n\t%s", qPrintable(missingFiles.join("\n\t")));
            }
        }
    }

    QDir dir(appRootPath);

    return dir.absolutePath();
}

QString binDir()
{
    return QApplication::applicationDirPath();
}

QString shareDir()
{
    QDir dir(appDir());
    dir.cd("share");
    dir.cd("moltaqa-lib");

    return dir.absolutePath();
}

QString stylesDir()
{
    QDir dir(shareDir());
    dir.cd("styles");

    return dir.absolutePath();
}

QString jsDir()
{
    QDir dir(shareDir());
    dir.cd("js");

    return dir.absolutePath();
}

QString localeDir()
{
    QDir dir(shareDir());
    dir.cd("locale");

    return dir.absolutePath();
}

QString dataDir()
{
    QDir dir(shareDir());
    dir.cd("data");

    return dir.absolutePath();
}

QString currentStyle(const QString &fileName)
{
    QSettings settings;
    QString defaultStyle = ML_DEFAULT_STYLE;
    QString currentStyle = settings.value("Style/name", defaultStyle).toString();

    QDir dir(stylesDir());
    if(!dir.cd(currentStyle)) {
        qWarning("currentStyle: style directory %s not found", qPrintable(currentStyle));
        if(!dir.cd(defaultStyle)) {
            qFatal("currentStyle: %s style directory not found", qPrintable(defaultStyle));
        }
    }

    if(fileName.size()) {
        ml_warn_on_fail(dir.exists(fileName),
                        "currentStyle: file" << fileName << "not found in" << dir.absolutePath());

        return dir.absoluteFilePath(fileName);
    }

    return dir.absolutePath();
}

QString currentStyleName()
{
    return Utils::Settings::get("Style/name", ML_DEFAULT_STYLE).toString();
}

QString id()
{
    if(!Utils::Settings::contains("Update/token"))
        Utils::Settings::set("Update/token", Utils::Rand::string(16, false));

    return Utils::Settings::get("Update/token").toString();
}

}

namespace Log {

void QueryError(const QSqlQuery &query, const char *file, int line)
{
    qCritical("[%s:%d] SQL error: %s",
              qPrintable(QFileInfo(file).fileName()), line,
              qPrintable(query.lastError().text()));
}

void DatabaseError(const QSqlDatabase &db, const char *file, int line)
{
    qCritical("[%s:%d] Database error: %s",
              qPrintable(QFileInfo(file).fileName()), line,
              qPrintable(db.lastError().text()));
}
}
