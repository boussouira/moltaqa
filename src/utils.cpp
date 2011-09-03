#include "utils.h"

#include <qsqlquery.h>
#include <qsqldatabase.h>
#include <qsqlerror.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qapplication.h>

namespace Utils {

int randInt(int smin, int smax)
{
    return (smin + (qrand() % (smax-smin+1)));
}

QString genBookName(QString path, bool fullPath, QString ext, QString namePrefix)
{
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

bool isLibraryPath(QString path)
{
    QDir dir(path);

    return (dir.exists() && dir.exists("books_index.db") && dir.exists("info.xml"));
}

void createIndexDB(QSqlQuery &query)
{
    query.exec("CREATE TABLE booksList ("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                     "bookID INTEGER , "
                     "bookType INTEGER , "
                     "bookFlags INTEGER , "
                     "bookCat INTEGER , "
                     "bookDisplayName TEXT , "
                     "bookFullName TEXT , "
                     "bookOtherNames TEXT , "
                     "bookInfo TEXT , "
                     "bookEdition TEXT , "
                     "bookPublisher TEXT , "
                     "bookMohaqeq TEXT , "
                     "authorName TEXT , "
                     "authorID INTEGER , "
                     "fileName TEXT , "
                     "bookFolder TEXT, "
                     "pdfID INTEGER)");

    query.exec("CREATE TABLE catList ("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                     "title TEXT , "
                     "description TEXT , "
                     "catOrder INTEGER , "
                     "parentID INTEGER)");

    query.exec("CREATE TABLE bookMeta("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                     "book_info TEXT, "
                     "add_time INTEGER, "
                     "update_time INTEGER)");

    query.exec("CREATE TABLE authorsList("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                     "name TEXT, "
                     "full_name TEXT, "
                     "die_year INTEGER, "
                     "info BLOB)");

    query.exec("CREATE TABLE tafassirList("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, " // TODO: should be AUTOINCREMENT?
                     "book_id INTEGER, "
                     "tafessir_name TEXT)");

    query.exec("CREATE TABLE ShareehMeta("
                     "mateen_book INTEGER, "
                     "mateen_id INTEGER, "
                     "shareeh_book INTEGER, "
                     "shareeh_id INTEGER)");
}

void createIndexDB(QString path)
{
    QDir dir(path);
    QString indexPath = dir.filePath("books_index.db");

    {
        QSqlDatabase indexDB = QSqlDatabase::addDatabase("QSQLITE", "createIndexDB");
        indexDB.setDatabaseName(indexPath);

        if (!indexDB.open()) {
            LOG_DB_ERROR(indexDB);
        }

        QSqlQuery query(indexDB);

        createIndexDB(query);
    }

    QSqlDatabase::removeDatabase("createIndexDB");
}
}

namespace App {
QString name()
{
    return QObject::tr("برنامج الكتبية");
}

const char *version()
{
    return APP_VERSION_STR;
}

int versionNumber()
{
    return APP_VERSION;
}

QString appDir()
{
    QDir dir(QApplication::applicationDirPath());
    dir.cdUp();

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
    dir.cd("alkotobiya");

    return dir.absolutePath();
}

QString stylesDir()
{
    QDir dir(shareDir());
    dir.cd("styles");

    return dir.absolutePath();
}

QString localeDir()
{
    QDir dir(shareDir());
    dir.cd("locale");

    return dir.absolutePath();
}
}

namespace Log {

void QueryError(QSqlQuery &query, const char *file, int line)
{
    qCritical("[%s:%d] SQL error: %s",
              qPrintable(QFileInfo(file).fileName()), line,
              qPrintable(query.lastError().text()));
}

void DatabaseError(QSqlDatabase &db, const char *file, int line)
{
    qCritical("[%s:%d] Database error: %s",
              qPrintable(QFileInfo(file).fileName()), line,
              qPrintable(db.lastError().text()));
}
}
