#include "sqlutils.h"
#include "utils.h"
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsqlerror.h>

namespace Utils {

DatabaseRemover::DatabaseRemover()
{
}

DatabaseRemover::~DatabaseRemover()
{
    if(!connectionName.isEmpty()) {
        //qDebug("Remove database: %s", qPrintable(connectionName));
        QSqlDatabase::removeDatabase(connectionName);
    }
}

QueryBuilder::QueryBuilder() :
    m_type(None),
    m_igonreExisting(false),
    m_dropExisting(false)
{
}

void QueryBuilder::setTableName(const QString &name)
{
    m_tableName = name;
}

void QueryBuilder::setQueryType(QueryType type)
{
    clear();
    m_type = type;
}

void QueryBuilder::setIgnoreExistingTable(bool ignore)
{
    m_igonreExisting = ignore;
}

void QueryBuilder::setDropExistingTable(bool drop)
{
    qWarning("QueryBuilder: Drop existingt table not implement");
    m_dropExisting = drop;
}

void QueryBuilder::addColumn(const QVariant &colName, const QVariant &colValue)
{
    m_colums.append(colName);
    m_values.append(colValue);
}

void QueryBuilder::addWhere(const QVariant &colName, const QVariant &colValue)
{
    m_whereColums.append(colName);
    m_whereValues.append(colValue);
}

QString QueryBuilder::query()
{
    ML_ASSERT_RET2(m_type != None, "QueryBuilder: Query type is not set", QString());
    ML_ASSERT_RET2(!m_tableName.isEmpty(), "QueryBuilder: Table name is not set", QString());
    ML_ASSERT_RET2(m_values.size() == m_colums.size(), "QueryBuilder: Columns and values doesn't match", QString());

    QString sql;
    if(m_type == Create) {
        /*
        if(m_dropExisting) {
            sql += "DROP TABLE IF EXISTS ";
            sql += m_tableName;
            sql += " ; ";
        }
        */

        sql += "CREATE TABLE ";
        if(m_igonreExisting)
            sql += "IF NOT EXISTS ";

        sql += m_tableName;
        sql += " (";

        for(int i=0; i<m_colums.size(); i++) {
            if(i)
                sql += ", ";

            sql += m_colums[i].toString();
            sql += " ";
            sql += m_values[i].toString();
        }

        sql += ")";
    } else if(m_type == Insert) {
        sql = "INSERT INTO ";
        sql += m_tableName;
        sql += " (";

        QString values = ") Values (";

        for(int i=0; i<m_colums.size(); i++) {
            if(i) {
                sql += ", ";
                values += ", ";
            }

            sql += m_colums[i].toString();
            values += "?";
        }

        sql += values;
        sql += ")";
    } else if(m_type == Update) {
        sql = "UPDATE ";
        sql += m_tableName;
        sql += " SET ";

        for(int i=0; i<m_colums.size(); i++) {
            if(i)
                sql += ", ";

            sql += m_colums[i].toString();
            sql += " = ";
            sql += "?";
        }

        if(!m_whereColums.isEmpty()) {
            ML_ASSERT_RET2(m_whereValues.size() == m_whereColums.size(),
                           "QueryBuilder: Where columns and values doesn't match", QString());

            sql += " WHERE ";

            for(int i=0; i<m_whereColums.size(); i++) {
                if(i)
                    sql += " AND ";

                sql += m_whereColums[i].toString();
                sql += " = ";
                sql += '?';
            }
        }
    }

    return sql;
}

void QueryBuilder::prepare(QSqlQuery &q)
{
    ML_ASSERT2(m_type != None, "QueryBuilder: Query type is not set");
    ML_ASSERT2(!m_tableName.isEmpty(), "QueryBuilder: Table name is not set");
    ML_ASSERT2(m_values.size() == m_colums.size(), "QueryBuilder: Columns and values doesn't match");

    QString sql;

    sql = query();
    ML_ASSERT2(!sql.isEmpty(), "QueryBuilder::prepare Query is empty");

    q.prepare(sql);

    ML_ASSERT(m_type != Create);

    for(int i=0; i<m_values.size(); i++)
        q.bindValue(i, m_values[i]);

    ML_ASSERT(m_type == Update);

    for(int i=0; i<m_whereValues.size(); i++)
        q.bindValue(i+m_values.size(), m_whereValues[i]);
}

bool QueryBuilder::exec(QSqlQuery &q)
{
    prepare(q);
    ML_ASSERT_RET2(q.exec(),
                   "QueryBuilder::exec Sql error:" << q.lastError().text() << "Query:" << q.lastQuery(),
                   false);

    return true;
}

void QueryBuilder::clear()
{
    m_type = None;
    m_colums.clear();
    m_values.clear();
    m_whereColums.clear();
    m_whereValues.clear();
}

}
