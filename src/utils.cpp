#include "utils.h"
#include "sqlutils.h"

#include <qsqlquery.h>
#include <qsqldatabase.h>
#include <qsqlerror.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qapplication.h>
#include <qdebug.h>
#include <qmessagebox.h>
#include <QDateTime>
#include <qsettings.h>
#include <qtreeview.h>
#include <qheaderview.h>

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

QString fileName(QString path, bool fullPath, QString ext, QString namePrefix)
{
    Rand::srand();

    QDir dir(path);
    QString fileName(namePrefix);
    QString chars("abcdefghijklmnpqrstuvwxyz0123456789");
    int smax = chars.size()-1;

    if(!ext.startsWith('.'))
        ext.prepend('.');


    for(int i=0; i<6; i++) {
        fileName.append(chars.at(number(0, smax)));
    }

    while(true) {
        if(dir.exists(fileName+ext)){
            fileName.append(chars.at(number(0, smax)));
        } else {
            break;
        }
    }

    if(fullPath)
        return dir.filePath(fileName+ext);
    else
        return fileName+ext;
}

QString string(int size)
{
    ml_return_val_on_fail2(size, "Rand::string size must be greater than 0", QString());

    Rand::srand();

    QString str;
    QString chars("abcdefghijklmnpqrstuvwxyzABCDEFGHIJKLMNPQRSTUVWXYZ0123456789");
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
    QString authorsDbPath = dir.filePath("authors.db");

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
        if(QFile::exists(authorsDbPath))
            qDebug("createDatabases: check authors database...");
        else
            qDebug("createDatabases: create authors database...");

        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "createDB.authors");
        db.setDatabaseName(authorsDbPath);

        if (!db.open()) {
            ml_warn_db_error(db);
        }

        QSqlQuery query(db);

        QueryBuilder q;
        q.setTableName("authors");
        q.setIgnoreExistingTable(true);
        q.setQueryType(QueryBuilder::Create);

        q.set("id", "INTEGER PRIMARY KEY NOT NULL");
        q.set("name", "TEXT");
        q.set("full_name", "TEXT");
        q.set("info", "TEXT");
        q.set("birth_year", "INTEGER");
        q.set("birth", "TEXT");
        q.set("death_year", "INTEGER");
        q.set("death", "TEXT");
        q.set("flags", "INTEGER");

        q.exec(query);
    }

    QSqlDatabase::removeDatabase("createDB.books");
    QSqlDatabase::removeDatabase("createDB.authors");
}
}

namespace Widget {

void save(QWidget *w, QString section)
{
    QSettings settings;
    settings.beginGroup("WidgetStat");
    settings.setValue(section + ".pos", w->pos());
    settings.setValue(section + ".size", w->size());
    settings.setValue(section + ".maximized", w->isMaximized());
    settings.endGroup();
}

void restore(QWidget *w, QString section, bool showMaximized)
{
    QSettings settings;
    settings.beginGroup("WidgetStat");

    QRect r(1337,1337,1337,1337);

    QPoint pos = settings.value(section + ".pos", r.topLeft()).toPoint();
    if(pos != r.topLeft())
        w->move(pos);

    QSize size = settings.value(section + ".size", r.size()).toSize();
    if(size != r.size())
        w->resize(size);

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
} // Widget

namespace Files {
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
} // Files

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

int versionNumber()
{
    return APP_VERSION;
}

QStringList checkDir(bool showWarnings)
{
    QStringList missingFiles;

    missingFiles << checkFiles(QStringList()
                               << "quran-meta.db"
                               << "rowat.db",
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
                QMessageBox::critical(0,
                                      App::name(),
                                      QObject::tr("لم يتم العثور على بعض الملفات في مجلد البرنامج""\n"
                                                  "من فضلك قم باعادة تتبيث البرنامج""\n"
                                                  "الملفات الناقصة:""\n") + missingFiles.join("\n"));

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
