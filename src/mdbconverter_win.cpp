#include "mdbconverter_win.h"
#include <qsqlerror.h>
#include <qsqlfield.h>
#include <qdatetime.h>
#include <qtextcodec.h>
#include <qsettings.h>
#include <qstringlist.h>
#include <qdebug.h>

MdbConverter::MdbConverter()
{
    QSettings settings;
    settings.beginGroup("General");
    m_tempFolder = settings.value("books_folder").toString()
                 + "/"
                 + settings.value("books_temp").toString();

}

MdbConverter::~MdbConverter()
{
}

QString MdbConverter::exportFromMdb(const QString &mdb_path, const QString &sql_path)
{
    QTime timer;
    timer.start();

    QString convert_path;
    if(!sql_path.isEmpty()){
        convert_path = sql_path;
    } else {
        convert_path = m_tempFolder;
        convert_path.append("/");
        convert_path.append(mdb_path.split("/").last());
        convert_path.append(".sqlite");
    }

    // MS Access database
    m_importDB = QSqlDatabase::addDatabase("QODBC", "ImportDB");
    m_importDB.setDatabaseName(QString("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=%1")
                               .arg(mdb_path));

    if (!m_importDB.open()) {
        qDebug("[%s:%d] Cannot open %s.", __FILE__, __LINE__, qPrintable(mdb_path));
        return QString();
    }

    // SQLite database
    m_exportDB = QSqlDatabase::addDatabase("QSQLITE", "exportDB");
    m_exportDB.setDatabaseName(convert_path);
    if (!m_exportDB.open()) {
        qDebug("[%s:%d] Cannot open %s.", __FILE__, __LINE__, qPrintable(convert_path));
        return QString();
    }
    m_exportQuery = QSqlQuery(m_exportDB);

    foreach(QString table, m_importDB.tables()){
        QSqlRecord rec = m_importDB.record(table);
        if(rec.count() > 0) {
            //qDebug() << "[*] Importing" << table << "...";
            generateTableSql(table, rec);
            m_exportDB.transaction();
            generateContentSql(table, rec, false);
            m_exportDB.commit();
        }
    }

    QString fileName = mdb_path.split("/").last().split(".").first();
    qDebug() << fileName << " -> " << timer.elapsed() << "ms";

    return convert_path;
}

void MdbConverter::generateTableSql(const QString &table, QSqlRecord &record)
{
    QString tableName(table);
    QString sql;

    if(tableName.contains(QRegExp("^[0-9]")))
        tableName[0] = '_';

    m_exportQuery.exec(QString("DROP TABLE IF EXISTS %1; ").arg(tableName));

    sql.append(QString("CREATE TABLE IF NOT EXISTS %1").arg(tableName));
    sql.append("\n(\n");
    for(int i=0; i<record.count(); i++){
        QSqlField field = record.field(i);
        sql.append(QString("\t%1\t%2").arg(field.name()).arg(getTypeName(field.type())));
        if(i != record.count()-1)
            sql.append(",\n");
    }
    sql.append("\n);\n\n");

//    m_sql.append(sql);
    m_exportQuery.exec(sql);
}

void MdbConverter::generateContentSql(const QString &table, QSqlRecord &record, bool filedNames)
{
    QString tableName(table);
    QSqlQuery query(m_importDB);

    if(tableName.contains(QRegExp("^[0-9]")))
        tableName[0] = '_';

    query.exec(QString("SELECT * FROM %1 ").arg(table));
    while(query.next()){
        QString fields;
        QString values;

        for(int i=0; i<record.count();i++){
            if(filedNames)
                fields.append(QString("'%1'").arg(record.fieldName(i)));
            values.append(QString("\"%1\"").arg(query.value(i).toString().replace('"', "\"\"")));
            if(i != record.count()-1){
                if(filedNames)
                    fields.append(", ");
                values.append(", ");
            }
        }
        if(filedNames)
            m_exportQuery.exec(QString("INSERT INTO %1 (%2) VALUES (%3);\n")
                         .arg(tableName).arg(fields).arg(values));
        else
            m_exportQuery.exec(QString("INSERT INTO %1 VALUES (%3);\n")
                         .arg(tableName).arg(values));
    }
}

QString MdbConverter::getTypeName(QVariant::Type type)
{
    if(QVariant::Int == type || QVariant::UInt == type)
        return "INTEGER";
    else if(QVariant::String == type)
        return "TEXT";
    else {
        qWarning() << "Warning: Type" << type << "is not handled.";
        return "TEXT";
    }
}
