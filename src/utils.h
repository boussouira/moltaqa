#ifndef COMMON_H
#define COMMON_H

#include <qdir.h>
#include <qfile.h>
#include <qdatetime.h>
#include <qdebug.h>
#include <qvariant.h>

#include "bookexception.h"

#define APP_VERSION_STR "0.9a4"
#define APP_UPDATE_REVISION 4

#define ML_DEFAULT_STYLE "default"

class QSqlQuery;
class QSqlDatabase;
class QTreeView;
class QDialog;
class QToolBar;

namespace Utils {

namespace Library {
bool isValidLibrary(const QString &path);
void createDatabases(const QString &path);
}

namespace Rand {
void srand();
int number(int smin, int smax);
QString string(int size, bool upperChar=true, bool lowerChar=true, bool numbers=true);
QString fileName(const QString &path, bool fullPath=false, QString namePrefix="book_", QString ext="mlb");
QString newBook(const QString &path);
}

namespace Widget {
void save(QWidget *w, QString section);
void restore(QWidget *w, QString section, bool showMaximized=false);

void save(QTreeView *tree, QString section, int columnCount=-1);
void restore(QTreeView *tree, QString section, QList<int> defaultWidth);

void save(QToolBar *bar);
void restore(QToolBar *bar);

void hideHelpButton(QWidget *w);
int exec(QDialog *dialog, QString section, bool hideHelp=true);
}

namespace Files {
bool move(const QString &sourcePath, const QString &destPath);
void removeDir(const QString &path);
bool copyData(QIODevice &inFile, QIODevice &outFile);
quint64 directorySize(const QString &path);
QString formatSize(quint64 size);
QString cleanFileName(QString fileName, bool removeSpace=false);
QString ensureFileExistsNot(QString path);
}

namespace Settings {
QVariant get(const QString &key, const QVariant &defaultValue=QVariant());
void set(const QString &key, const QVariant &defaultValue);
void remove(const QString &key);
bool contains(const QString &key);
}
}

namespace App {
QString name();
const char *version();

QStringList checkDir(bool showWarning);
QStringList checkFiles(QStringList files, QDir dir, bool showWarning);

QString appDir();
QString binDir();
QString shareDir();
QString stylesDir();
QString jsDir();
QString localeDir();
QString dataDir();

QString currentStyle(const QString &fileName=QString());

QString id();
}

namespace Log {

void QueryError(const QSqlQuery &query, const char *file, int line);
void DatabaseError(const QSqlDatabase &db, const char *file, int line);
}

// TODO: move this defines to an other header file

#define _u(x) QString::fromUtf8(x)

#define ml_theme_icon(name, defaultIcon) QIcon::fromTheme(name, QIcon(defaultIcon))

#define ml_benchmark(desc, code) do {QTime bmt;bmt.start();code;qDebug() << desc << "take" << bmt.elapsed() << "ms";} while(false)
#define ml_benchmark_start() QTime bmt2;bmt2.start();
#define ml_benchmark_elapsed(desc) qDebug() << desc << "take" << bmt2.elapsed() << "ms";bmt2.restart();

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

#define ml_set_instance(name, val) ml_warn_on_fail(!name, qPrintable(QString("[%1:%2]").arg(QFileInfo(__FILE__).fileName()).arg(__LINE__)) << #name " is already set"); name = val;

#define ml_throw_on_fail(con, what) if(!(con)) { throw BookException(what); }
#define ml_throw_on_fail2(con, what, file) if(!(con)) { throw BookException(what, file); }

#define ml_throw_on_query_exec_fail(query) if(!query.exec()) { throw BookException(query.lastError().text(), QString("[%1:%2]").arg(QFileInfo(__FILE__).fileName()).arg(__LINE__)); }
#define ml_throw_on_query_exec_fail2(query, sql) query.prepare(sql); ml_throw_on_query_exec_fail(query);

#define ml_throw_on_db_open_fail(db) if(!db.open()) { throw BookException(db.lastError().text(), QString("[%1:%2]").arg(QFileInfo(__FILE__).fileName()).arg(__LINE__)); }

#define ml_delete(p) delete p; p=0;
#define ml_delete_check(p) if(p) { delete p; p=0; }

#endif // COMMON_H
