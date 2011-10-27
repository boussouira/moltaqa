#ifndef COMMON_H
#define COMMON_H

#include <qdir.h>
#include <qfile.h>

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
bool isLibraryPath(QString path);
void createIndexDB(QSqlQuery &query);
void createIndexDB(QString path);
}

namespace App {
QString name();
const char *version();
int versionNumber();

QString appDir();
QString binDir();
QString shareDir();
QString stylesDir();
QString localeDir();
}

namespace Sql {
QString buildLikeQuery(QString text, QString column);
}

namespace Log {

void QueryError(const QSqlQuery &query, const char *file, int line);
void DatabaseError(const QSqlDatabase &db, const char *file, int line);
}

#define LOG_DB_ERROR(db) Log::DatabaseError(db, __FILE__, __LINE__);
#define LOG_SQL_ERROR(query) Log::QueryError(query, __FILE__, __LINE__);
#define LOG_SQL_P_ERROR(query) Log::QueryError((*query), __FILE__, __LINE__);

#endif // COMMON_H
