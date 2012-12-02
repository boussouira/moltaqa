#include "searchmanager.h"
#include "mainwindow.h"
#include "librarymanager.h"
#include "libraryinfo.h"
#include "utils.h"
#include <qdir.h>
#include <qstringlistmodel.h>

SearchManager::SearchManager(QObject *parent) :
    DatabaseManager(parent)
{
    QDir dataDir(MW->libraryInfo()->dataDir());
    setDatabasePath(dataDir.filePath("search.db"));

    openDatabase();
}

void SearchManager::loadModels()
{
}

void SearchManager::clear()
{
}

QList<SearchFieldInfo> SearchManager::getFieldNames()
{
    QList<SearchFieldInfo> list;
    QSqlQuery query(m_db);

    query.prepare("SELECT id, name FROM fields");
    if(!query.exec())
        ml_warn_query_error(query);

    while(query.next()) {
        SearchFieldInfo field;
        field.fieldID = query.value(0).toInt();
        field.name = query.value(1).toString();

        list.append(field);
    }

    return list;
}

bool SearchManager::addField(const QString &name, const QList<int> &list)
{
    ml_return_val_on_fail2(name.size(), "SearchManager::addField name is empty", false);
    ml_return_val_on_fail2(list.size(), "SearchManager::addField books list is empty", false);

    QSqlQuery query(m_db);

    QueryBuilder q;
    q.setTableName("fields", QueryBuilder::Insert);
    q.set("name", name);

    ml_return_val_on_fail(q.exec(query), false);

    transaction();

    int fid = query.lastInsertId().toInt();
    ml_return_val_on_fail2(fid, "SearchManager::addField can't get last inserted id", false);;

    for(int i=0; i<list.size(); i++) {
        query.prepare("INSERT INTO fieldsBooks (field, bookID) "
                         "VALUES(?, ?)");
        query.bindValue(0, fid);
        query.bindValue(1, list[i]);

        ml_query_exec(query);
    }

    return commit();
}

QList<int> SearchManager::getFieldBooks(int fid)
{
    QList<int> list;
    QSqlQuery query(m_db);

    query.prepare("SELECT bookID FROM fieldsBooks "
                     "WHERE field = ?");
    query.bindValue(0, fid);
    query.exec();

    while(query.next()) {
        list.append(query.value(0).toInt());
    }

    return list;
}


void SearchManager::removeField(int fid)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM fields WHERE id = ?");
    query.bindValue(0, fid);
    if(query.exec()) {
        query.prepare("DELETE FROM fieldsBooks WHERE field = ?");
        query.bindValue(0, fid);
        query.exec();
    }
}

void SearchManager::setFieldName(const QString &name, int fid)
{
    ml_return_on_fail2(name.size(), "SearchManager::setFieldName name is empty");

    QSqlQuery query(m_db);
    QueryBuilder q;

    q.setTableName("fields", QueryBuilder::Update);
    q.set("name", name);
    q.where("id", fid);

    q.exec(query);
}

QStringListModel *SearchManager::getSavedSearchModel()
{
    QStringListModel *model = new QStringListModel();
    QSqlQuery query(m_db);

    query.prepare("SELECT query FROM savedSearch "
                  "ORDER BY id");

    ml_query_exec(query);

    QStringList words;
    while(query.next())
        words.append(query.value(0).toString());

    model->setStringList(words);

    return model;
}

void SearchManager::saveSearchQueries(QStringList list)
{
    ml_return_on_fail(list.size());

    QSqlQuery query(m_db);

    transaction();

    QueryBuilder q;
    q.setTableName("savedSearch");

    foreach(QString str, list) {
        if(str.isEmpty())
            continue;

        q.setQueryType(QueryBuilder::Select);
        q.where("query", str);
        q.exec(query);

        if(query.next())
            continue;

        q.setQueryType(QueryBuilder::Insert);
        q.set("query", str);

        q.exec(query);
    }

    commit();
}

void SearchManager::removeSavedQueries()
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM savedSearch");

    ml_query_exec(query);
}
