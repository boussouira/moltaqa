#include "sqlutils.h"
#include "utils.h"
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsqlerror.h>

DatabaseRemover::DatabaseRemover()
{
}

DatabaseRemover::~DatabaseRemover()
{
    for(int i=0; i<m_connectionNames.size(); i++) {
        //qDebug("DatabaseRemover: remove database %s", qPrintable(m_connectionNames[i]));
        QSqlDatabase::removeDatabase(m_connectionNames[i]);
    }
}

void DatabaseRemover::removeDatabase(const QSqlDatabase &db)
{
    //ML_ASSERT2(db.isOpen(), "DatabaseRemover::removeDatabase db is not currently open");

    m_connectionNames.append(db.connectionName());
}

QueryBuilder::QueryBuilder() :
    m_type(None),
    m_igonreExisting(false),
    m_dropExisting(false),
    m_limit(-1)
{
}

void QueryBuilder::setTableName(const QString &name)
{
    m_tableName = name;
}

void QueryBuilder::setTableName(const QString &name, QueryBuilder::QueryType type)
{
    clear();
    m_tableName = name;
    m_type = type;
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

void QueryBuilder::select(const QVariant &colName)
{
    ML_ASSERT2(m_type == Select, "QueryBuilder::addColumn add column "
               "without value should be only in select query");

    m_colums.append(colName);
}

void QueryBuilder::set(const QVariant &colName, const QVariant &colValue)
{
    m_colums.append(colName);
    m_values.append(colValue);
}

void QueryBuilder::where(const QVariant &colName, const QVariant &colValue)
{
    m_whereColums.append(colName);
    m_whereValues.append(colValue);
}

void QueryBuilder::orderBy(const QString &colName, QueryBuilder::Order order)
{
    m_orderColumns.append(QString("%1 %2")
                          .arg(colName)
                          .arg((order == Asc ? "ASC" : "DESC")));
}

void QueryBuilder::limit(int _limit)
{
    m_limit = _limit;
}

QString QueryBuilder::query()
{
    ML_ASSERT_RET2(m_type != None, "QueryBuilder::query Query type is not set", QString());
    ML_ASSERT_RET2(!m_tableName.isEmpty(), "QueryBuilder::query Table name is not set", QString());
    ML_ASSERT_RET2(m_type == Select || m_values.size() == m_colums.size(),
                   "QueryBuilder::query Columns and values doesn't match", QString());

    if(!m_whereColums.isEmpty()) {
        ML_ASSERT_RET2(m_whereValues.size() == m_whereColums.size(),
                       "QueryBuilder: Where columns and values doesn't match", QString());
    }

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
    } else if(m_type == Insert || m_type == Replace) {
        if(m_type == Insert)
            sql = "INSERT INTO ";
        else
            sql = "INSERT OR REPLACE INTO ";

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
            sql += " WHERE ";

            for(int i=0; i<m_whereColums.size(); i++) {
                if(i)
                    sql += " AND ";

                sql += m_whereColums[i].toString();
                sql += " = ";
                sql += '?';
            }
        }
    } else if(m_type == Select) {
        sql = "SELECT ";

        if(!m_colums.isEmpty()) {
            for(int i=0; i<m_colums.size(); i++) {
                if(i)
                    sql += ", ";

                sql += m_colums[i].toString();
            }
        } else {
            sql += "*";
        }

        sql += " FROM ";
        sql += m_tableName;

        if(!m_whereColums.isEmpty()) {
            sql += " WHERE ";
            for(int i=0; i<m_whereColums.size(); i++) {
                if(i)
                    sql += " AND ";

                sql += m_whereColums[i].toString();
                sql += " = ";
                sql += '?';
            }
        }

        if(!m_orderColumns.isEmpty()) {
            sql += " ORDER BY ";

            for(int i=0; i<m_orderColumns.size(); i++) {
                if(i)
                    sql += ", ";

                sql += m_orderColumns[i];
            }
        }

        if(m_limit != -1)
            sql += QString(" LIMIT %1").arg(m_limit);

    } else {
        qWarning("QueryBuilder: Query type %d is not handled", m_type);
    }

    return sql;
}

void QueryBuilder::prepare(QSqlQuery &q)
{
    ML_ASSERT2(m_type != None, "QueryBuilder::prepare Query type is not set");
    ML_ASSERT2(!m_tableName.isEmpty(), "QueryBuilder::prepare Table name is not set");
    ML_ASSERT2(m_type == Select || m_values.size() == m_colums.size(),
               "QueryBuilder::prepare Columns and values doesn't match");

    QString sql;

    sql = query();
    ML_ASSERT2(!sql.isEmpty(), "QueryBuilder::prepare Query is empty");

    q.prepare(sql);

    if(m_type != Create) {
        for(int i=0; i<m_values.size(); i++)
            q.bindValue(i, m_values[i]);

        if(m_type == Update || m_type == Select) {
            for(int i=0; i<m_whereValues.size(); i++)
                q.bindValue(i+m_values.size(), m_whereValues[i]);
        }
    }
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
    m_orderColumns.clear();
    m_limit = -1;
}
