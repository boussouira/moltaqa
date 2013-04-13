#include "tarajemrowatmanager.h"
#include "authorsmanager.h"
#include "libraryinfo.h"
#include "librarymanager.h"
#include "mainwindow.h"
#include "modelenums.h"
#include "stringutils.h"
#include "timeutils.h"
#include "utils.h"

#include <qdir.h>
#include <qstandarditemmodel.h>

TarajemRowatManager::TarajemRowatManager(QObject *parent) :
    DatabaseManager(parent)
{
    QDir dataDir(MW->libraryInfo()->dataDir());
    setDatabasePath(dataDir.filePath("rowat.db"));

    openDatabase();
}

TarajemRowatManager::~TarajemRowatManager()
{
    clear();
}

void TarajemRowatManager::loadModels()
{
}

void TarajemRowatManager::clear()
{
    m_rowat.clear();
}

void TarajemRowatManager::reloadModels()
{
    clear();
    loadModels();
}

QStandardItemModel *TarajemRowatManager::getRowatModel()
{

    QSqlQuery query(m_db);
    query.prepare("SELECT id, name FROM rowat ORDER BY id");

    ml_query_exec(query);

    QStandardItemModel *model = new QStandardItemModel();
    model->setHorizontalHeaderLabels(QStringList() << tr("الرواة"));

    while(query.next()) {
        QStandardItem *item = new QStandardItem();
        item->setText(Utils::String::abbreviate(query.value(1).toString(), 100));
        item->setToolTip(query.value(1).toString());
        item->setData(query.value(0).toInt(), ItemRole::authorIdRole);

        model->appendRow(item);
    }

    return model;
}

int TarajemRowatManager::rowatCount()
{
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM rowat");
    if(query.exec() && query.next())
        return query.value(0).toInt();

    return 0;
}

RawiInfo::Ptr TarajemRowatManager::getRawiInfo(int rawiID)
{
    RawiInfo::Ptr rawi = m_rowat.value(rawiID);
    if(rawi)
        return rawi;

    QSqlQuery query(m_db);
    query.prepare("SELECT id, name, laqab, birth_year, birth, death_year, death, "
                  "tabaqa, rowat, rotba_hafed, rotba_zahabi, sheok, talamid, tarejama "
                  "FROM rowat WHERE id = ?");

    query.bindValue(0, rawiID);

    ml_query_exec(query);

    if(query.next()) {
        rawi = RawiInfo::Ptr(new RawiInfo());
        rawi->id = query.value(0).toInt();
        rawi->name = query.value(1).toString();
        rawi->tabaqa = query.value(7).toString();
        rawi->laqab = query.value(2).toString();
        rawi->rowat = query.value(8).toString();
        rawi->rotba_hafed = query.value(9).toString();
        rawi->rotba_zahabi = query.value(10).toString();

        if(!query.value(3).isNull()) {
            rawi->birthYear = query.value(3).toInt();
            rawi->birthStr = query.value(4).toString();
        }

        if(rawi->birthStr.isEmpty() && !rawi->unknowBirth())
            rawi->birthStr = Utils::Time::hijriYear(rawi->birthYear);

        if(!query.value(5).isNull()) {
            rawi->deathYear = query.value(5).toInt();
            rawi->deathStr = query.value(6).toString();
        }

        if(rawi->deathStr.isEmpty() && !rawi->unknowDeath())
            rawi->deathStr = Utils::Time::hijriYear(rawi->deathYear);

        rawi->sheok = query.value(11).toString();
        rawi->talamid = query.value(12).toString();
        rawi->tarejama = query.value(13).toString();

        m_rowat[rawi->id] = rawi;
    }

    return rawi;
}

bool TarajemRowatManager::updateRawi(RawiInfo::Ptr rawi)
{
    QSqlQuery query(m_db);

    QueryBuilder q;
    q.setTableName("rowat");
    q.setQueryType(QueryBuilder::Update);

    q.set("name", rawi->name);
    q.set("laqab", rawi->laqab);

    q.set("birth_year", rawi->unknowBirth() ? QVariant(QVariant::Int) : rawi->birthYear);
    q.set("birth", rawi->unknowBirth() ? QVariant(QVariant::String) : rawi->birthStr);

    q.set("death_year", rawi->unknowBirth() ? QVariant(QVariant::Int) : rawi->deathYear);
    q.set("death", rawi->unknowBirth() ? QVariant(QVariant::String) : rawi->deathStr);

    q.set("tabaqa", rawi->tabaqa);
    q.set("rowat", rawi->rowat);

    q.set("rotba_hafed", rawi->rotba_hafed);
    q.set("rotba_zahabi", rawi->rotba_zahabi);

    q.set("sheok", rawi->sheok);
    q.set("talamid", rawi->talamid);
    q.set("tarejama", rawi->tarejama);

    q.where("id", rawi->id);

    ml_return_val_on_fail(q.exec(query), false);

    m_rowat[rawi->id] = rawi;
    return true;
}

int TarajemRowatManager::addRawi(RawiInfo::Ptr rawi)
{
    QMutexLocker locker(&m_mutex);

    if(!rawi->id)
        rawi->id = getNewRawiID();

    QSqlQuery query(m_db);

    QueryBuilder q;
    q.setTableName("rowat");
    q.setQueryType(QueryBuilder::Insert);

    q.set("id", rawi->id);
    q.set("name", rawi->name);
    q.set("laqab", rawi->laqab);

    q.set("birth_year", rawi->unknowBirth() ? QVariant(QVariant::Int) : rawi->birthYear);
    q.set("birth", rawi->unknowBirth() ? QVariant(QVariant::String) : rawi->birthStr);

    q.set("death_year", rawi->unknowBirth() ? QVariant(QVariant::Int) : rawi->deathYear);
    q.set("death", rawi->unknowBirth() ? QVariant(QVariant::String) : rawi->deathStr);

    q.set("tabaqa", rawi->tabaqa);
    q.set("rowat", rawi->rowat);

    q.set("rotba_hafed", rawi->rotba_hafed);
    q.set("rotba_zahabi", rawi->rotba_zahabi);

    q.set("sheok", rawi->sheok);
    q.set("talamid", rawi->talamid);
    q.set("tarejama", rawi->tarejama);

    ml_return_val_on_fail(q.exec(query), 0);

    m_rowat[rawi->id] = rawi;

    return rawi->id;
}

bool TarajemRowatManager::removeRawi(int rawiID)
{
    m_query.prepare("DELETE FROM rowat WHERE id = ?");
    m_query.bindValue(0, rawiID);

    if(m_query.exec()) {
        m_rowat.remove(rawiID);
        return true;
    } else {
        ml_warn_query_error(m_query);
        return false;
    }
}

int TarajemRowatManager::getNewRawiID()
{
    int rawiID = 0;
    do {
        rawiID = Utils::Rand::number(11111, 99999);
    } while(getRawiInfo(rawiID));

    return rawiID;
}
