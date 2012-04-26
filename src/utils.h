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
class QTreeView;

namespace Utils {

namespace Library {
bool isValidLibrary(const QString &path);
void createDatabases(const QString &path);
}

namespace Rand {
void srand();
int number(int smin, int smax);
QString string(int size);
QString fileName(QString path, bool fullPath=false, QString ext="alb", QString namePrefix="book_");
}

namespace Widget {
void save(QWidget *w, QString section);
void restore(QWidget *w, QString section, bool showMaximized=false);

void save(QTreeView *tree, QString section, int columnCount=-1);
void restore(QTreeView *tree, QString section, QList<int> defaultWidth);
}

namespace Files {
void removeDir(const QString &path);
}

}

namespace App {
QString name();
const char *version();
int versionNumber();

QStringList checkDir(bool showWarning);
QStringList checkFiles(QStringList files, QDir dir, bool showWarning);

QString appDir();
QString binDir();
QString shareDir();
QString stylesDir();
QString jsDir();
QString localeDir();
QString dataDir();
}

namespace Log {

void QueryError(const QSqlQuery &query, const char *file, int line);
void DatabaseError(const QSqlDatabase &db, const char *file, int line);
}

// TODO: move this defines to an other header file

#define ml_benchmark(desc, code) do {QTime bmt;bmt.start();code;qDebug(desc " take %d ms", bmt.elapsed());} while(false)
#define ml_benchmark_start() QTime bmt2;bmt2.start();
#define ml_benchmark_elapsed(desc) qDebug(desc " take %d ms", bmt2.restart());

#define ml_warn_db_error(db) Log::DatabaseError(db, __FILE__, __LINE__)
#define ml_warn_query_error(query) Log::QueryError(query, __FILE__, __LINE__)
#define ml_warn_query_error2(query) Log::QueryError((*query), __FILE__, __LINE__)

#define ml_open_db(db) if(!db.open()) { ml_warn_db_error(db); }
#define ml_query_exec(query) if(!query.exec()) { ml_warn_query_error(query); }

#define ml_return_on_fail(con)    if(!(con)) return
#define ml_return_on_fail2(con, msg)    if(!(con)) {qWarning() << msg ; return;}

#define ml_return_val_on_fail(con, ret)    if(!(con)) return ret
#define ml_return_val_on_fail2(con, msg, ret)    if(!(con)) {qWarning() << msg ; return ret;}

#define ml_warn_on_fail(con, msg) if(!(con)) {qWarning() << msg ;}

#define ml_delete(p) delete p; p=0;
#define ml_delete_check(p) if(p) { delete p; p=0; }

#endif // COMMON_H
