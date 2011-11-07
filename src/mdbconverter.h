#ifndef MDBCONVERTER_UNIX_H
#define MDBCONVERTER_UNIX_H

#include <qobject.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>

#include <mdbtools.h>
#include <glib.h>

#include <sqlutils.h>

#define is_text_type(x) (x==MDB_TEXT || x==MDB_MEMO || x==MDB_SDATETIME)
using namespace std;

class QSqlQuery;

class MdbConverter : public QObject
{
public:
    MdbConverter(bool cache=false);
    ~MdbConverter();
    QString exportFromMdb(const QString &mdb_path, const QString &sql_path=QString());
    static void removeConvertedDB(QString shamelaDB);
    static void removeAllConvertedDB();

protected:
    void generateTableSchema(MdbCatalogEntry *entry);
    void getTableContent(MdbHandle *mdb, MdbCatalogEntry *entry, bool fieldsName=true);
    void print_col(QString &std, gchar *col_val, int quote_text, int col_type, char *quote_char, char *escape_char);
    char *sanitizeName(char *str);

protected:
    Utils::DatabaseRemover m_remover;
    QSqlDatabase m_bookDB;
    QSqlQuery m_bookQuery;
    QString m_tempFolder;
    bool m_cache;
};

#endif // MDBCONVERTER_UNIX_H
