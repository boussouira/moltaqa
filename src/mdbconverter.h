#ifndef MDBCONVERTER_UNIX_H
#define MDBCONVERTER_UNIX_H

#include <qobject.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>

#include <glib.h>
#include <mdbtools.h>

#include <sqlutils.h>

using namespace std;

class QSqlQuery;

class MdbConverter : public QObject
{
    Q_OBJECT

public:
    MdbConverter(bool cache=false);
    ~MdbConverter();
    QString exportFromMdb(const QString &mdb_path, const QString &sql_path=QString());
    static void removeConvertedDB(QString shamelaDB);
    static void removeAllConvertedDB();

    static void init();
    static void exit();

protected:
    void generateTableSchema(MdbCatalogEntry *entry);
    void getTableContent(MdbHandle *mdb, MdbCatalogEntry *entry, bool fieldsName=true);
    void print_col(QString &std, gchar *col_val, bool quote_text, int col_type);
    QString sanitizeName(QString str);

protected:
    DatabaseRemover m_remover;
    QSqlDatabase m_bookDB;
    QSqlQuery m_bookQuery;
    QString m_tempFolder;
    QHash<QString, QString> m_sanitizedName;
    bool m_cache;
};

#endif // MDBCONVERTER_UNIX_H
