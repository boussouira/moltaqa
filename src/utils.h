#ifndef COMMON_H
#define COMMON_H

#include <qdir.h>
#include <qfile.h>

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

QString genBookName(QString path, bool fullPath=false, QString ext="alb");
int randInt(int smin, int smax);

QString arPlural(int count, int word, bool html=false);
QString secondsToString(int milsec, bool html=false);

}

namespace Log {

void QueryError(QSqlQuery &query, const char *file, int line);
void DatabaseError(QSqlDatabase &db, const char *file, int line);
}

#define LOG_DB_ERROR(db) Log::DatabaseError(db, __FILE__, __LINE__);
#define LOG_SQL_ERROR(query) Log::QueryError(query, __FILE__, __LINE__);
#define LOG_SQL_P_ERROR(query) Log::QueryError((*query), __FILE__, __LINE__);

#endif // COMMON_H
