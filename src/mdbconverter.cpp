#include "mdbconverter.h"
#include "mainwindow.h"
#include "libraryinfo.h"
#include "utils.h"

#include <stdio.h>
#include <qsqlerror.h>
#include <qdatetime.h>
#include <qtextcodec.h>
#include <qstringlist.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qhash.h>

#define is_text_type(x) (x==MDB_TEXT || x==MDB_OLE || x==MDB_MEMO || x==MDB_DATETIME || x==MDB_BINARY || x==MDB_REPID)

static QHash<QString, QString> m_converted;

const char *get_col_type(int col_type)
{
    switch (col_type)
    {
    case MDB_INT:
    case MDB_LONGINT:
    case MDB_NUMERIC:
        return "INT";
        break;
    case MDB_BINARY:
    case MDB_BYTE:
        return "BLOB";
        break;
    default:
        return "TEXT";
        break;
    }
}

MdbConverter::MdbConverter(bool cache) : m_cache(cache)
{
    m_tempFolder = MW->libraryInfo()->tempDir();
}

MdbConverter::~MdbConverter()
{
}

QString MdbConverter::exportFromMdb(const QString &mdb_path, const QString &sql_path)
{
    if(m_cache) {
          QString shamelaDB(mdb_path);
          shamelaDB = shamelaDB.toLower();
          QString convertDB = m_converted.value(shamelaDB, QString());

          if(convertDB.size()) {
              qDebug() << "MdbConverter: Database" << shamelaDB << "already converted";
              return convertDB;
          }
    }

    QString convert_path;
    if(sql_path.size()){
        convert_path = sql_path;
    } else {
        convert_path = Utils::Rand::fileName(m_tempFolder, true, "mdb_", "sqlite");
    }

    MdbHandle *mdb;

    // open the database
    if (!(mdb = mdb_open (qPrintable(mdb_path), MDB_NOFLAGS))) {
        qCritical() << "MdbConverter: Couldn't open mdb database at"
                    << mdb_path;

        return QString();
    }


    // read the catalog
    if (!mdb_read_catalog (mdb, MDB_TABLE)) {
        qCritical() << "MdbConverter: File" << mdb_path
                    << "does not appear to be an Access database.";

        return QString();
    }

    m_bookDB = QSqlDatabase::addDatabase("QSQLITE", QString("bok2sql_%1")
                                         .arg(qChecksum(qPrintable(convert_path), convert_path.size())));
    m_bookDB.setDatabaseName(convert_path);

    if (!m_bookDB.open()) {
        ml_warn_db_error(m_bookDB);
        return QString();
    }

    m_bookQuery = QSqlQuery(m_bookDB);

    // loop over each entry in the catalog
    MdbCatalogEntry *entry;
    unsigned int num_catalog = mdb->num_catalog;

    for (unsigned int i=0; i < num_catalog; i++) {
        entry = (MdbCatalogEntry*) g_ptr_array_index (mdb->catalog, i);

        if (entry->object_type != MDB_TABLE || mdb_is_system_table(entry))
            continue;

        generateTableSchema(entry);
        m_bookDB.transaction();
        getTableContent(mdb, entry, false);
        m_bookDB.commit();
    }

    mdb_close(mdb);

    m_converted[mdb_path.toLower()] = convert_path;

    m_remover.removeDatabase(m_bookDB);

    return convert_path;
}

void MdbConverter::getTableContent(MdbHandle *mdb, MdbCatalogEntry *entry, bool fieldsName)
{
    MdbTableDef *table;
    MdbColumn *col;

    char **bound_values;
    char *null_str = g_strdup("NULL");
    int  *bound_lens;

    table = mdb_read_table(entry);
    ml_return_on_fail2(table, "getTableContent: Table %s does not exist in this database." << entry->object_name);

    mdb_read_columns(table);
    mdb_rewind_table(table);

    bound_values = (char **) g_malloc(table->num_cols * sizeof(char *));
    bound_lens = (int *) g_malloc(table->num_cols * sizeof(int));
    for (unsigned int j=0;j<table->num_cols;j++) {
        bound_values[j] = (char *) g_malloc0(MDB_BIND_SIZE);
        mdb_bind_column(table, j+1, bound_values[j], &bound_lens[j]);
    }

    while(mdb_fetch_row(table)) {
        QString sqlCmd;

        sqlCmd.append("INSERT INTO ");
        sqlCmd.append(sanitizeName(entry->object_name));
        if(fieldsName) {
            sqlCmd.append(" (");
            for (unsigned int k=0;k<table->num_cols;k++) {
                if (k>0)
                    sqlCmd.append(", ");
                col=(MdbColumn*) g_ptr_array_index(table->columns,k);
                sqlCmd.append(sanitizeName(col->name));
            }
            sqlCmd.append(")");
        }
        sqlCmd.append(" VALUES (");

        for (unsigned int p=0;p<table->num_cols;p++) {
            col=(MdbColumn*) g_ptr_array_index(table->columns,p);
            if ((col->col_type == MDB_OLE)
                && ((p==0) || (col->cur_value_len))) {
                mdb_ole_read(mdb, col, bound_values[p], MDB_BIND_SIZE);
            }
            if (p>0)
                sqlCmd.append(", ");

            if (!bound_lens[p])
                print_col(sqlCmd, null_str, false, col->col_type);
             else
                print_col(sqlCmd, bound_values[p], true, col->col_type);
        }
       sqlCmd.append(");");

       if(!m_bookQuery.exec(sqlCmd)) {
           ml_warn_query_error(m_bookQuery);
       }
    }

    for (unsigned int n=0;n<table->num_cols;n++)
        g_free(bound_values[n]);

    g_free(bound_values);
    g_free(null_str);
    g_free(bound_lens);
    mdb_free_tabledef(table);

}

void MdbConverter::generateTableSchema(MdbCatalogEntry *entry)
{
    MdbTableDef *table;
    MdbColumn *col;
    QString sqlCmd;

    m_bookQuery.exec(QString("DROP TABLE IF EXISTS %1; ").arg(sanitizeName(entry->object_name)));

    // create the table
    sqlCmd.append("CREATE TABLE IF NOT EXISTS ");
    sqlCmd.append(sanitizeName(entry->object_name));
    sqlCmd.append(" (");

    table = mdb_read_table (entry);

    // get the columns
    mdb_read_columns (table);

    // loop over the columns, dumping the names and types
    for (unsigned int i = 0; i < table->num_cols; i++) {
        col = (MdbColumn*) g_ptr_array_index (table->columns, i);

        sqlCmd.append(sanitizeName(col->name));
        sqlCmd.append(" ");
        sqlCmd.append(get_col_type(col->col_type));

        if (i < table->num_cols - 1)
            sqlCmd.append( ", ");

    }

    sqlCmd.append( ");");
    if(!m_bookQuery.exec(sqlCmd)){
        ml_warn_query_error(m_bookQuery);
    }

    mdb_free_tabledef (table);
}

QString MdbConverter::sanitizeName(QString str)
{
    if(m_sanitizedName.contains(str))
        return m_sanitizedName[str];

    QString clearStr = str;
    if(!clearStr[0].isLetter())
        clearStr[0] = '_';

    m_sanitizedName[str] = clearStr;

    return clearStr;
}

void MdbConverter::print_col(QString &str,gchar *col_val, bool quote_text, int col_type)
{
    QString value = QString::fromUtf8(col_val);
    if (quote_text && is_text_type(col_type)) {
        str.append('"');
        str.append(value.replace('"', "\"\""));
        str.append('"');
    } else {
        str.append(value);
    }
}

void MdbConverter::removeConvertedDB(QString shamelaDB)
{
    QString convertedDB = m_converted.value(shamelaDB.toLower(), QString());
    if(convertedDB.size()) {
        QFile::remove(convertedDB);
        m_converted.remove(shamelaDB.toLower());
    }
}

void MdbConverter::removeAllConvertedDB()
{
    QHashIterator<QString, QString> i(m_converted);
    while (i.hasNext()) {
        i.next();
        QFile::remove(i.value());
    }

    m_converted.clear();
}

void MdbConverter::init()
{
    mdb_init();
}

void MdbConverter::exit()
{
    mdb_exit();
}
