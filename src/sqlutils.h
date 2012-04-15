#ifndef SQLUTILS_H
#define SQLUTILS_H

#include <qstring.h>
#include <qstringlist.h>
#include <qvariant.h>

class QSqlDatabase;
class QSqlQuery;

class DatabaseRemover
{
public:
    DatabaseRemover();
    ~DatabaseRemover();

    inline void removeDatabase(const QString &name)
    {
        m_connectionNames.append(name);
    }

    void removeDatabase(const QSqlDatabase &db);

protected:
    QStringList m_connectionNames;
};

class QueryBuilder
{
public:
    QueryBuilder();

    enum QueryType {
        None,
        Select,
        Create,
        Insert,
        Update,
        Replace
    };

    enum Order {
        Asc,
        Desc
    };

    void setTableName(const QString &name);
    void setTableName(const QString &name, QueryType type);
    void setQueryType(QueryType type);
    void setIgnoreExistingTable(bool ignore);
    void setDropExistingTable(bool drop);
    void select(const QVariant &colName);
    void set(const QVariant &colName, const QVariant &colValue);
    void where(const QVariant &colName, const QVariant &colValue);
    void orderBy(const QString &colName, Order order=Asc);
    void limit(int _limit);

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
    QList<QString> m_orderColumns;
    bool m_igonreExisting;
    bool m_dropExisting;
    int m_limit;
};

#endif // SQLUTILS_H
