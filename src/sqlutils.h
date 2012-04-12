#ifndef SQLUTILS_H
#define SQLUTILS_H

#include <qstring.h>
#include <qvariant.h>

class QSqlQuery;

namespace Utils {

class DatabaseRemover
{
public:
    DatabaseRemover();
    ~DatabaseRemover();

    QString connectionName;
};

class QueryBuilder
{
public:
    QueryBuilder();

    enum QueryType {
        None,
        Create,
        Insert,
        Update,
        Replace
    };

    void setTableName(const QString &name);
    void setTableName(const QString &name, QueryType type);
    void setQueryType(QueryType type);
    void setIgnoreExistingTable(bool ignore);
    void setDropExistingTable(bool drop);
    void addColumn(const QVariant &colName, const QVariant &colValue);
    void addWhere(const QVariant &colName, const QVariant &colValue);

    QString query();

    void prepare(QSqlQuery &q);
    bool exec(QSqlQuery &q);

    void clear();

protected:
    QueryType m_type;
    QString m_tableName;
    QList<QVariant> m_colums;
    QList<QVariant> m_values;
    QList<QVariant> m_whereColums;
    QList<QVariant> m_whereValues;
    bool m_igonreExisting;
    bool m_dropExisting;
};

}

#endif // SQLUTILS_H
