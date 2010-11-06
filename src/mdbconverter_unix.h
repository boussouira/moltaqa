#ifndef MDBCONVERTER_UNIX_H
#define MDBCONVERTER_UNIX_H

#include <qobject.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>

#include <mdbtools.h>
#include <glib.h>

#define is_text_type(x) (x==MDB_TEXT || x==MDB_MEMO || x==MDB_SDATETIME)
using namespace std;

class QSqlQuery;

class MdbConverter : public QObject
{
public:
    MdbConverter();
    ~MdbConverter();
    QString exportFromMdb(const QString &mdb_path, const QString &sql_path=QString());

protected:
    void getTableContent(MdbHandle *mdb, MdbCatalogEntry *entry, bool fieldsName=true);
    void getTableSchema(MdbHandle *mdb, char *tabname);
    int getTables(MdbHandle *mdb, char *buffer[]);
    void print_col(QString &std, gchar *col_val, int quote_text, int col_type, char *quote_char, char *escape_char);
    void generateTableSchema(MdbCatalogEntry *entry);
    char *sanitizeName(char *str);

protected:
    QSqlDatabase m_bookDB;
    QSqlQuery m_bookQuery;
    QString m_tempFolder;
};

#endif // MDBCONVERTER_UNIX_H
