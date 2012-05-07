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
    //ml_return_on_fail2(db.isOpen(), "DatabaseRemover::removeDatabase db is not currently open");

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

void QueryBuilder::select(const QString &colName)
{
    ml_return_on_fail2(m_type == Select, "QueryBuilder::addColumn add column "
               "without value should be only in select query");

    m_colums.append(colName);
}

void QueryBuilder::set(const QString &colName, const QVariant &colValue)
{
    m_colums.append(colName);
    m_values.append(colValue);
}

void QueryBuilder::where(const QString &colName, const QVariant &colValue)
{
    m_whereColums.append(colName);
    m_whereValues.append(colValue);
}

void QueryBuilder::like(const QString &colName, const QString &colValue)
{
    QStringList list = colValue.split(" ", QString::SkipEmptyParts);
    QString sql;

    for(int i=0; i < list.count(); i++) {
        if(i>0)
            sql.append(" AND ");

        sql.append(QString("%1 LIKE '%%2%'").arg(colName).arg(list.at(i)));
    }

    m_likeValues.append(sql);
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
    ml_return_val_on_fail2(m_type != None, "QueryBuilder::query Query type is not set", QString());
    ml_return_val_on_fail2(m_tableName.size(), "QueryBuilder::query Table name is not set", QString());
    ml_return_val_on_fail2(m_type == Select || m_type == Delete || m_values.size() == m_colums.size(),
                   "QueryBuilder::query Columns and values doesn't match", QString());

    if(m_whereColums.size()) {
        ml_return_val_on_fail2(m_whereValues.size() == m_whereColums.size(),
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

            sql += m_colums[i];
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

            sql += m_colums[i];
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

            sql += m_colums[i];
            sql += " = ";
            sql += "?";
        }

        if(m_whereColums.size()) {
            sql += " WHERE ";

            for(int i=0; i<m_whereColums.size(); i++) {
                if(i)
                    sql += " AND ";

                sql += m_whereColums[i];
                sql += " = ";
                sql += '?';
            }
        }
    } else if(m_type == Select) {
        sql = "SELECT ";

        if(m_colums.size()) {
            for(int i=0; i<m_colums.size(); i++) {
                if(i)
                    sql += ", ";

                sql += m_colums[i];
            }
        } else {
            sql += "*";
        }

        sql += " FROM ";
        sql += m_tableName;

        if(m_whereColums.size() || m_likeValues.size())
            sql += " WHERE ";

        if(m_whereColums.size()) {
            for(int i=0; i<m_whereColums.size(); i++) {
                if(i)
                    sql += " AND ";

                sql += m_whereColums[i];
                sql += " = ";
                sql += '?';
            }
        }

        if(m_likeValues.size()) {
            if(m_whereColums.size())
                sql += " AND ";

            for(int i=0; i<m_likeValues.size(); i++) {
                if(i)
                    sql += " AND ";

                sql += m_likeValues[i];
            }
        }

        if(m_orderColumns.size()) {
            sql += " ORDER BY ";

            for(int i=0; i<m_orderColumns.size(); i++) {
                if(i)
                    sql += ", ";

                sql += m_orderColumns[i];
            }
        }

        if(m_limit != -1)
            sql += QString(" LIMIT %1").arg(m_limit);

    } else if (m_type == Delete){
        sql = "DELETE FROM ";
        sql += m_tableName;

        if(m_whereColums.size()) {
            sql += " WHERE ";
            for(int i=0; i<m_whereColums.size(); i++) {
                if(i)
                    sql += " AND ";

                sql += m_whereColums[i];
                sql += " = ";
                sql += '?';
            }
        }
    } else {
        qWarning("QueryBuilder: Query type %d is not handled", m_type);
    }

    return sql;
}

void QueryBuilder::prepare(QSqlQuery &q)
{
    ml_return_on_fail2(m_type != None, "QueryBuilder::prepare Query type is not set");
    ml_return_on_fail2(m_tableName.size(), "QueryBuilder::prepare Table name is not set");
    ml_return_on_fail2(m_type == Select || m_type == Delete || m_values.size() == m_colums.size(),
               "QueryBuilder::prepare Columns and values doesn't match");

    QString sql;

    sql = query();
    ml_return_on_fail2(sql.size(), "QueryBuilder::prepare Query is empty");

    q.prepare(sql);

    if(m_type != Create) {
        if(m_type != Delete) {
            for(int i=0; i<m_values.size(); i++)
                q.bindValue(i, m_values[i]);
        }

        if(m_type == Update || m_type == Select || m_type == Delete) {
            for(int i=0; i<m_whereValues.size(); i++)
                q.bindValue(i+m_values.size(), m_whereValues[i]);
        }
    }
}

bool QueryBuilder::exec(QSqlQuery &q)
{
    prepare(q);
    ml_return_val_on_fail2(q.exec(),
                   "QueryBuilder::exec Sql error:" << q.lastError().text() << "Query:" << q.lastQuery(),
                   false);

    return true;
}

bool QueryBuilder::exec(QSqlDatabase &db)
{
    QSqlQuery query(db);
    return exec(query);
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
