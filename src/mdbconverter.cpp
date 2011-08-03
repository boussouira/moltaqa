#include "mdbconverter.h"
#include <qsqlerror.h>
#include <qdatetime.h>
#include <qtextcodec.h>
#include <qsettings.h>
#include <qstringlist.h>
#include <qdebug.h>
#include <qdir.h>

MdbConverter::MdbConverter()
{
    QSettings settings;
    QDir dir(settings.value("General/library_dir").toString());
    if(!dir.exists("temp"))
        dir.mkdir("temp");

    m_tempFolder = dir.filePath("temp");
}

MdbConverter::~MdbConverter()
{
}

QString MdbConverter::exportFromMdb(const QString &mdb_path, const QString &sql_path)
{
    QTime timer;
    timer.start();
    MdbHandle *mdb;

    putenv(strdup("MDB_JET3_CHARSET=cp1256"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("windows-1256"));

    QString convert_path;
    if(!sql_path.isEmpty()){
        convert_path = sql_path;
    } else {
        convert_path = m_tempFolder;
        convert_path.append("/");
        convert_path.append(mdb_path.split("/").last());
        convert_path.append(".sqlite");
    }

    // initialize the library
    mdb_init();

    // open the database
    if (!(mdb = mdb_open (qPrintable(mdb_path), MDB_NOFLAGS))) {
        qDebug() << "Couldn't open database.";
        return QString();
    }


    // read the catalog
    if (!mdb_read_catalog (mdb, MDB_TABLE)) {
        qDebug() << "File does not appear to be an Access database.";
        return QString();
    }

    m_bookDB = QSqlDatabase::addDatabase("QSQLITE", "bok2sql");
    m_bookDB.setDatabaseName(convert_path);

    if (!m_bookDB.open()) {
        qDebug() << "Cannot open database.";
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
        //qDebug() << "[*] Importing" << entry->object_name << "...";
        generateTableSchema(entry);
        m_bookDB.transaction();
        getTableContent(mdb, entry, false);
        m_bookDB.commit();
    }

    QString fileName = mdb_path.split("/").last().split(".").first();
    qDebug() << fileName << " -> " << timer.elapsed() << "ms";

    mdb_close(mdb);
    mdb_exit();

    return convert_path;
}

int MdbConverter::getTables(MdbHandle *mdb, char *buffer[])
{
    // loop over each entry in the catalog
    MdbCatalogEntry *entry;

    unsigned int num_catalog = mdb->num_catalog;
    int tables_count=0;

    for (unsigned int i=0; i < num_catalog; i++) {
        entry = (MdbCatalogEntry*) g_ptr_array_index (mdb->catalog, i);

        if (entry->object_type != MDB_TABLE || mdb_is_system_table(entry))
            continue;

        buffer[tables_count++] = entry->object_name;
    }

    return tables_count;
}

void MdbConverter::getTableSchema(MdbHandle *mdb, char *tabname)
{
    for (unsigned int i=0; i < mdb->num_catalog; i++) {
        MdbCatalogEntry *entry = (MdbCatalogEntry*) g_ptr_array_index (mdb->catalog, i);
        if (entry->object_type == MDB_TABLE) {
            if ((tabname && !strcmp(entry->object_name, tabname))
                || (!tabname && mdb_is_user_table(entry))) {
                generateTableSchema(entry);
            }
        }
    }
}

void MdbConverter::getTableContent(MdbHandle *mdb, MdbCatalogEntry *entry, bool fieldsName)
{
    MdbTableDef *table;
    MdbColumn *col;

    char **bound_values;
    char *quote_char = (char *) g_strdup("\"");
    char *escape_char = NULL;
    int  *bound_lens;

    table = mdb_read_table(entry);
    if (!table) {
        qDebug() << "Error: Table" << entry->object_name << "does not exist in this database.";
        return;
    }

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
                print_col(sqlCmd, g_strdup("NULL"), 0, col->col_type, quote_char, escape_char);
             else
                print_col(sqlCmd, bound_values[p], 1, col->col_type, quote_char, escape_char);
        }
       sqlCmd.append(");");

       if(!m_bookQuery.exec(sqlCmd)) {
           qDebug() << "[2] SQL error: (" << m_bookQuery.lastError().text() << ")";
           qDebug() << sqlCmd;
       }
    }

    for (unsigned int n=0;n<table->num_cols;n++)
        g_free(bound_values[n]);

    g_free(bound_values);
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
        qDebug() << "[1] SQL error: (" << m_bookQuery.lastError().text() << ")";
        qDebug() << sqlCmd;
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
