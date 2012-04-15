#include "mdbconverter.h"
#include "mainwindow.h"
#include "libraryinfo.h"
#include "utils.h"

#include <stdio.h>
#include <qsqlerror.h>
#include <qdatetime.h>
#include <qtextcodec.h>
#include <qsettings.h>
#include <qstringlist.h>
#include <qdebug.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qhash.h>

static int m_time = 0;
static qint64 m_size = 0;
static QHash<QString, QString> m_converted;

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

          if(!convertDB.isEmpty()) {
              qDebug() << "Database:" << shamelaDB << "already converted";
              return convertDB;
          }
    }

    QTime timer;
    timer.start();

    QString convert_path;
    if(!sql_path.isEmpty()){
        convert_path = sql_path;
    } else {
        convert_path = Utils::Rand::fileName(m_tempFolder, true, "sqlite", "mdb_");
    }

    MdbHandle *mdb;

    // initialize the library
    mdb_init();

    // open the database
    if (!(mdb = mdb_open (qPrintable(mdb_path), MDB_NOFLAGS))) {
        qCritical() << "Couldn't open mdb database at" << mdb_path;
        return QString();
    }


    // read the catalog
    if (!mdb_read_catalog (mdb, MDB_TABLE)) {
        qCritical("File does not appear to be an Access database.");
        return QString();
    }

    m_bookDB = QSqlDatabase::addDatabase("QSQLITE", QString("bok2sql_%1")
                                         .arg(qChecksum(qPrintable(convert_path), convert_path.size())));
    m_bookDB.setDatabaseName(convert_path);

    if (!m_bookDB.open()) {
        LOG_DB_ERROR(m_bookDB);
        return QString();
    }

    m_bookQuery = QSqlQuery(m_bookDB);

    QFileInfo info(mdb_path);

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

    m_size = info.size();
    m_time = timer.elapsed();

    qDebug() << "Converting" << info.fileName() << " take " << m_time << "ms";

    mdb_close(mdb);
    mdb_exit();

    m_converted[mdb_path.toLower()] = convert_path;

    m_remover.connectionName = m_bookDB.connectionName();

    return convert_path;
}

void MdbConverter::getTableContent(MdbHandle *mdb, MdbCatalogEntry *entry, bool fieldsName)
{
    MdbTableDef *table;
    MdbColumn *col;

    char **bound_values;
    char *quote_char = g_strdup("\"");
    char *null_str = g_strdup("NULL");
    char *escape_char = NULL;
    int  *bound_lens;

    table = mdb_read_table(entry);
    ML_ASSERT2(table, "getTableContent: Table %s does not exist in this database." << entry->object_name);

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
                print_col(sqlCmd, null_str, 0, col->col_type, quote_char, escape_char);
             else
                print_col(sqlCmd, bound_values[p], 1, col->col_type, quote_char, escape_char);
        }
       sqlCmd.append(");");

       if(!m_bookQuery.exec(sqlCmd)) {
           LOG_SQL_ERROR(m_bookQuery);
       }
    }

    for (unsigned int n=0;n<table->num_cols;n++)
        g_free(bound_values[n]);

    g_free(bound_values);
    g_free(quote_char);
    g_free(null_str);
    g_free(bound_lens);
    mdb_free_tabledef(table);

}

void MdbConverter::generateTableSchema(MdbCatalogEntry *entry)
{
    MdbTableDef *table;
    MdbHandle *mdb = entry->mdb;
    MdbColumn *col;
    QString sqlCmd;

    /* Sqlite types */
    QStringList sqlite_types;
    sqlite_types << "Text" << "char" << "int" << "int" << "int" << "float"
            << "float" << "float" << "date" << "varchar" << "varchar"
            << "varchar" << "text" << "blob" << "text" << "numeric" << "numeric";

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
        sqlCmd.append(qPrintable(sqlite_types.at(col->col_type)));

        if (mdb_coltype_takes_length(mdb->default_backend, col->col_type)) {
            if (col->col_size == 0)
                sqlCmd.append(" (255)");
            else
                sqlCmd.append(QString(" (%1) ").arg(col->col_size));
        }

        if (i < table->num_cols - 1)
            sqlCmd.append( ", ");

    }

    sqlCmd.append( ");");
    if(!m_bookQuery.exec(sqlCmd)){
        LOG_SQL_ERROR(m_bookQuery);
    }

    mdb_free_tabledef (table);
}

char* MdbConverter::sanitizeName(char *str)
{
    static char namebuf[256];
    char *p = namebuf;

    while (*str) {
        *p = isalnum(*str) ? *str : '_';
        p++;
        str++;
    }

    // if the first char is a number
    if(isdigit(*namebuf))
        *namebuf = '_' ;

    *p = 0;

    return namebuf;
}

void MdbConverter::print_col(QString &str,gchar *col_val, int quote_text, int col_type, char *quote_char, char *escape_char)
{
    QString value = QString::fromLocal8Bit(col_val);
    if (quote_text && is_text_type(col_type)) {
        str.append(quote_char);
        if (!escape_char)
            value.replace(quote_char[0], QString("%1%1").arg(quote_char));
        else
            value.replace(quote_char[0], QString("%1%2").arg(escape_char).arg(quote_char));

        str.append(value);
        str.append(quote_char);
    } else {
        str.append(value);
    }
}

void MdbConverter::removeConvertedDB(QString shamelaDB)
{
    QString convertedDB = m_converted.value(shamelaDB.toLower(), QString());
    if(!convertedDB.isEmpty()) {
        QFile::remove(convertedDB);
        qDebug() << "Delete converted book" << convertedDB;
        m_converted.remove(convertedDB);
    }
}

void MdbConverter::removeAllConvertedDB()
{
    QHashIterator<QString, QString> i(m_converted);
    while (i.hasNext()) {
        i.next();
        qDebug() << "Remove:" << i.value();
        QFile::remove(i.value());
    }
}
