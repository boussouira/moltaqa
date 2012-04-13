#ifndef COMMON_H
#define COMMON_H

#include <qdir.h>
#include <qfile.h>
#include <qdatetime.h>
#include <qdebug.h>

#define APP_VERSION_STR "0.9.0"
#define APP_VERSION 0x000900
#define APP_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))


class QSqlQuery;
class QSqlDatabase;

namespace Utils {

bool isLibraryPath(QString path);
void createIndexDB(QString path);

namespace Rand {
QString fileName(QString path, bool fullPath=false, QString ext="alb", QString namePrefix="book_");
int number(int smin, int smax);
}

namespace Widget {
void savePosition(QWidget *w, QString section);
void restorePosition(QWidget *w, QString section, bool showMaximized=false);
}

namespace Files {
void removeDir(const QString &path);
}

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

// TODO: move this defines to an other header file

#define ML_BENCHMARK(desc, code) do {QTime bmt;bmt.start();code;qDebug(desc " take %d ms", bmt.elapsed());} while(false)
#define ML_BENCHMARK_START() QTime bmt2;bmt2.start();
#define ML_BENCHMARK_ELAPSED(desc) qDebug(desc " take %d ms", bmt2.elapsed());

#define LOG_DB_ERROR(db) Log::DatabaseError(db, __FILE__, __LINE__)
#define LOG_SQL_ERROR(query) Log::QueryError(query, __FILE__, __LINE__)
#define LOG_SQL_P_ERROR(query) Log::QueryError((*query), __FILE__, __LINE__)

#define ML_OPEN_DB(db) if(!db.open()) { LOG_DB_ERROR(db); }
#define ML_QUERY_EXEC(query) if(!query.exec()) { LOG_SQL_ERROR(query); }

#define ML_ASSERT(con)    if(!(con)) return
#define ML_ASSERT2(con, msg)    if(!(con)) {qWarning() << msg ; return;}

#define ML_ASSERT_RET(con, ret)    if(!(con)) return ret
#define ML_ASSERT_RET2(con, msg, ret)    if(!(con)) {qWarning() << msg ; return ret;}

#define ML_DELETE(p) delete p; p=0;
#define ML_DELETE_CHECK(p) if(p) { delete p; p=0; }

#endif // COMMON_H
