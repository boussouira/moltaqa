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

static QString appRootPath;

namespace Utils {

int randInt(int smin, int smax)
{
    int rVal = (smin + (rand() % (smax-smin+1)));
    return qBound(smin, rVal, smax);
}

QString genBookName(QString path, bool fullPath, QString ext, QString namePrefix)
{
    srand(uint(QDateTime::currentDateTime().toMSecsSinceEpoch() & 0xFFFFFF));

    QDir dir(path);
    QString fileName(namePrefix);
    QString chars("abcdefghijklmnpqrstuvwxyz0123456789");
    int smax = chars.size()-1;

    if(!ext.startsWith('.'))
        ext.prepend('.');


    for(int i=0; i<6; i++) {
        fileName.append(chars.at(randInt(0, smax)));
    }

    while(true) {
        if(dir.exists(fileName+ext)){
            fileName.append(chars.at(randInt(0, smax)));
        } else {
            break;
        }
    }

    if(fullPath)
        return dir.filePath(fileName+ext);
    else
        return fileName+ext;
}

QString arPlural(int count, int word, bool html)
{
    QStringList list;
    QString str;

    if(word == Plural::SECOND)
        list << QObject::tr("ثانية")
             << QObject::tr("ثانيتين")
             << QObject::tr("ثوان")
             << QObject::tr("ثانية");
    else if(word == Plural::MINUTE)
        list << QObject::tr("دقيقة")
             << QObject::tr("دقيقتين")
             << QObject::tr("دقائق")
             << QObject::tr("دقيقة");
    else if(word == Plural::HOUR)
        list << QObject::tr("ساعة")
             << QObject::tr("ساعتين")
             << QObject::tr("ساعات")
             << QObject::tr("ساعة");
    else if(word == Plural::BOOK)
        list << QObject::tr("كتاب واحد")
             << QObject::tr("كتابين")
             << QObject::tr("كتب")
             << QObject::tr("كتابا");
    else if(word == Plural::AUTHOR)
        list << QObject::tr("مؤلف واحد")
             << QObject::tr("مؤلفيين")
             << QObject::tr("مؤلفيين")
             << QObject::tr("مؤلفا");
    else if(word == Plural::CATEGORIE)
        list << QObject::tr("قسم واحد")
             << QObject::tr("قسمين")
             << QObject::tr("أقسام")
             << QObject::tr("قسما");
    else if(word == Plural::FILES)
        list << QObject::tr("ملف واحد")
             << QObject::tr("ملفين")
             << QObject::tr("ملفات")
             << QObject::tr("ملفا");

    if(count <= 1)
        str = list.at(0);
    else if(count == 2)
        str = list.at(1);
    else if (count > 2 && count <= 10)
        str = QString("%1 %2").arg(count).arg(list.at(2));
    else if (count > 10)
        str = QString("%1 %2").arg(count).arg(list.at(3));
    else
        str = QString();

    return html ? QString("<strong>%1</strong>").arg(str) : str;
}

QString secondsToString(int milsec, bool html)
{
    QString time;

    int seconde = (int) ((milsec / 1000) % 60);
    int minutes = (int) (((milsec / 1000) / 60) % 60);
    int hours   = (int) (((milsec / 1000) / 60) / 60);

    if(hours > 0){
        time.append(arPlural(hours, Plural::HOUR, html));
        time.append(QObject::tr(" و "));
    }

    if(minutes > 0 || hours > 0) {
        time.append(arPlural(minutes, Plural::MINUTE, html));
        time.append(QObject::tr(" و "));
    }

    time.append(arPlural(seconde, Plural::SECOND, html));

    return time;
}

QString abbreviate(QString str, int size) {
        if (str.length() <= size-3)
                return str;

        str.simplified();
        int index = str.lastIndexOf(' ', size-3);
        if (index <= -1)
                return "";

        return str.left(index).append("...");
}

QString hijriYear(int hYear)
{
    if(hYear <= 0)
        return QObject::tr("%1 م").arg(hijriToGregorian(hYear));
    else if(hYear >= 99999)
        return QObject::tr("معاصر");
    else
        return QObject::tr("%1 هـ").arg(hYear);
}

QString arClean(QString text)
{
    text = text.simplified();
    text.replace(QRegExp("[\\x0622\\x0623\\x0625]"), QChar(0x0627));//ALEFs
    text.replace(QChar(0x0629), QChar(0x0647)); //TAH_MARBUTA -> HEH
    text.replace(QChar(0x0649), QChar(0x064A)); //YAH -> ALEF MAKSOURA
    text.remove(QRegExp("[\\x064B-\\x0653]"));

    return text;
}

bool arCompare(QString first, QString second)
{
    return arClean(first) == arClean(second);
}

bool arContains(QString src, QString text)
{
    QString cleanSrc = arClean(src);
    QString cleanText = arClean(text);

    return cleanSrc.contains(cleanText);
}

bool arFuzzyContains(QString first, QString second)
{
    QString cleanFirst= arClean(first);
    QString cleanSecond = arClean(second);

    if(cleanFirst.size() < cleanSecond.size())
        return cleanSecond.contains(cleanFirst);
    else
        return cleanFirst.contains(cleanSecond);
}

int hijriToGregorian(int hYear)
{
    return (hYear + 622) - (hYear / 33);
}

int gregorianToHijri(int gYear)
{
    return  (gYear - 622) + ((gYear - 622) / 32);
}

bool isLibraryPath(QString path)
{
    QDir dir(path);

    return (dir.exists() && dir.exists("info.xml"));
}

void createIndexDB(QString path)
{
    QDir dir(path);
    if(!dir.exists("data"))
        dir.mkdir("data");

    dir.cd("data");

    QString booksDbPath = dir.filePath("books.db");
    QString authorsDbPath = dir.filePath("authors.db");
    QString rowatDbPath = dir.filePath("rowat.db");

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "createDB.books");
        db.setDatabaseName(booksDbPath);

        if (!db.open()) {
            LOG_DB_ERROR(db);
        }

        QSqlQuery query(db);

        Utils::QueryBuilder q;
        q.setIgnoreExistingTable(true);

        q.setTableName("books", Utils::QueryBuilder::Create);

        q.set("id", "INTEGER PRIMARY KEY NOT NULL");
        q.set("title", "TEXT");
        q.set("type", "INT");
        q.set("authorID", "INT");
        q.set("author", "TEXT");
        q.set("info", "TEXT");
        q.set("bookFlags", "INT");
        q.set("indexFlags", "INT");
        q.set("filename", "TEXT");

        q.exec(query);

        q.setTableName("history", Utils::QueryBuilder::Create);

        q.set("id", "INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL");
        q.set("book", "INT");
        q.set("page", "INT");
        q.set("open_date", "INT");

        q.exec(query);

        q.setTableName("last_open", Utils::QueryBuilder::Create);

        q.set("book", "INTEGER PRIMARY KEY NOT NULL");
        q.set("page", "INT");
        q.set("open_date", "INT");

        q.exec(query);
    }

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "createDB.authors");
        db.setDatabaseName(authorsDbPath);

        if (!db.open()) {
            LOG_DB_ERROR(db);
        }

        QSqlQuery query(db);

        Utils::QueryBuilder q;
        q.setTableName("authors");
        q.setIgnoreExistingTable(true);
        q.setQueryType(Utils::QueryBuilder::Create);

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
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "createDB.rowat");
        db.setDatabaseName(rowatDbPath);

        if (!db.open()) {
            LOG_DB_ERROR(db);
        }

        QSqlQuery query(db);

        Utils::QueryBuilder q;
        q.setTableName("rowat");
        q.setIgnoreExistingTable(true);
        q.setQueryType(Utils::QueryBuilder::Create);

        q.set("id", "INTEGER PRIMARY KEY NOT NULL");
        q.set("name", "TEXT");
        q.set("laqab", "TEXT");

        q.set("birth_year", "INT");
        q.set("birth", "TEXT");

        q.set("death_year", "INT");
        q.set("death", "TEXT");

        q.set("tabaqa", "TEXT");
        q.set("rowat", "TEXT");

        q.set("rotba_hafed", "TEXT");
        q.set("rotba_zahabi", "TEXT");

        q.set("sheok", "TEXT");
        q.set("talamid", "TEXT");
        q.set("tarejama", "TEXT");

        q.exec(query);
    }

    QSqlDatabase::removeDatabase("createDB.books");
    QSqlDatabase::removeDatabase("createDB.authors");
    QSqlDatabase::removeDatabase("createIndexDB.rowat");
}

void saveWidgetPosition(QWidget *w, QString section)
{
    QSettings settings;
    settings.beginGroup(section);
    settings.setValue("pos", w->pos());
    settings.setValue("size", w->size());
    settings.setValue("maximized", w->isMaximized());
    settings.endGroup();
}

void restoreWidgetPosition(QWidget *w, QString section, bool showMaximized)
{
    QSettings settings;
    settings.beginGroup(section);

    QRect r(1337,1337,1337,1337);

    QPoint pos = settings.value("pos", r.topLeft()).toPoint();
    if(pos != r.topLeft())
        w->move(pos);

    QSize size = settings.value("size", r.size()).toSize();
    if(size != r.size())
        w->resize(size);

    if(settings.value("maximized", showMaximized).toBool())
        w->showMaximized();

    settings.endGroup();
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
            qDebug() << "Can't delete:" << path;
    }
}

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

bool checkDir(bool showWarnings)
{
    bool ret = true;
    ret &= checkFiles(QStringList()
                      << "quran-meta.db",
                      dataDir(),
                      showWarnings);

    ret &= checkFiles(QStringList()
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

    ret &=  checkFiles(QStringList()
                       << "qt_ar.qm",
                       localeDir(),
                       showWarnings);

    return ret;
}

bool checkFiles(QStringList files, QDir dir, bool showWarnings)
{
    bool ret = true;

    foreach(QString fileName, files) {
        if(!dir.exists(fileName)) {
            if(showWarnings)
                qWarning() << "checkFiles: File doesn't exist"
                           << dir.filePath(fileName);
            ret = false;
        }
    }

    return ret;
}

QString appDir()
{
    if(appRootPath.isEmpty()) {
        QDir dir(QApplication::applicationDirPath());
        dir.cdUp();

        appRootPath = dir.absolutePath();
        if(!checkDir(false)) {
//            qWarning() << "Can't find some files at" << appRootPath;
//            qWarning() << "Check if we can use current working directory...";
            appRootPath = QDir::currentPath();
            if(!checkDir(true)) {
                QMessageBox::critical(0,
                                     App::name(),
                                     QObject::tr("لم يتم العثور على بعض الملفات في مجلد البرنامج"
                                                 "\n"
                                                 "من فضلك قم باعادة تتبيث البرنامج"));

                qFatal("Some files are messing");
            } else {
//                qDebug() << "Using current working directory:" << appRootPath;
            }
        } else {
//            qDebug() << "Using application directory:" << appRootPath;
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
}

namespace Sql {

QString buildLikeQuery(QString text, QString column)
{
    QStringList list = text.split(" ", QString::SkipEmptyParts);
    QString sql;

    for(int i=0; i < list.count(); i++) {
        if(i>0)
            sql.append(" AND ");

        sql.append(QString("%1 LIKE '%%2%'").arg(column).arg(list.at(i)));
    }

    return sql;
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
