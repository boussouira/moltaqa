#ifndef COMMON_H
#define COMMON_H

#include <qdir.h>
#include <qfile.h>
#include <qdatetime.h>

#define APP_VERSION_STR "0.9.0"
#define APP_VERSION 0x000900
#define APP_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))


class QSqlQuery;
class QSqlDatabase;

namespace Plural {
enum {
    SECOND  = 1,
    MINUTE,
    HOUR,
    BOOK,
    AUTHOR,
    CATEGORIE,
    FILES
};
}

namespace Utils {
/**
  Generate a random file name with book extension
  @param path Books folder path
  @param fullPath if false return file name only, if true return the full path
*/

QString genBookName(QString path, bool fullPath=false, QString ext="alb", QString namePrefix="book_");
int randInt(int smin, int smax);

QString arPlural(int count, int word, bool html=false);
QString secondsToString(int milsec, bool html=false);
QString abbreviate(QString str, int size);
QString hijriYear(int hYear);
QString arClean(QString text);
bool arCompare(QString first, QString second);
bool arContains(QString src, QString text);
bool arFuzzyContains(QString first, QString second);

int hijriToGregorian(int hYear);
int gregorianToHijri(int gYear);

bool isLibraryPath(QString path);
void createIndexDB(QSqlQuery &query);
void createIndexDB(QString path);

void saveWidgetPosition(QWidget *w, QString section);
void restoreWidgetPosition(QWidget *w, QString section, bool showMaximized=false);
}

namespace App {
QString name();
const char *version();
int versionNumber();

bool checkDir(bool showWarning);
bool checkFiles(QStringList files, QDir dir, bool showWarning);

QString appDir();
QString binDir();
QString shareDir();
QString stylesDir();
QString jsDir();
QString localeDir();
QString dataDir();
}

namespace Sql {
QString buildLikeQuery(QString text, QString column);
}

namespace Log {

void QueryError(const QSqlQuery &query, const char *file, int line);
void DatabaseError(const QSqlDatabase &db, const char *file, int line);
}


#define BENCHMARK(desc, code) do {QTime t;t.start();code;qDebug(desc " take %d ms", t.elapsed());} while(false)

#define LOG_DB_ERROR(db) Log::DatabaseError(db, __FILE__, __LINE__);
#define LOG_SQL_ERROR(query) Log::QueryError(query, __FILE__, __LINE__);
#define LOG_SQL_P_ERROR(query) Log::QueryError((*query), __FILE__, __LINE__);

#endif // COMMON_H
