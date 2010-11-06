#ifndef MDBCONVERTER_WIN_H
#define MDBCONVERTER_WIN_H

#include <qobject.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsqlrecord.h>
#include <qvariant.h>

class QSqlQuery;

class MdbConverter : public QObject
{
public:
    MdbConverter();
    ~MdbConverter();
    QString exportFromMdb(const QString &mdb_path, const QString &sql_path=QString());

protected:
    void generateTableSql(const QString &table, QSqlRecord &record);
    void generateContentSql(const QString &table, QSqlRecord &record, bool filedNames=true);
    QString getTypeName(QVariant::Type type);

protected:
    QSqlDatabase m_importDB;
    QSqlDatabase m_exportDB;
    QSqlQuery m_importQuery;
    QSqlQuery m_exportQuery;
    QString m_tempFolder;
};

#endif // MDBCONVERTER_WIN_H
