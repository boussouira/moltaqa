#include "tarajemrowatmanager.h"
#include "mainwindow.h"
#include "librarymanager.h"
#include "libraryinfo.h"
#include "modelenums.h"
#include "utils.h"
#include "authorsmanager.h"

#include <qdir.h>
#include <qstandarditemmodel.h>

TarajemRowatManager *m_instance = 0;

TarajemRowatManager::TarajemRowatManager(QObject *parent) :
    DatabaseManager(parent)
{
    QDir dataDir(MW->libraryInfo()->dataDir());
    setDatabasePath(dataDir.filePath("rowat.db"));

    m_instance = this;

    openDatabase();
    loadModels();
}

TarajemRowatManager::~TarajemRowatManager()
{
    m_instance = 0;
    clear();
}

TarajemRowatManager *TarajemRowatManager::instance()
{
    return m_instance;
}

void TarajemRowatManager::loadModels()
{
    QSqlQuery query(m_db);
    query.prepare("SELECT id, name FROM rowat ORDER BY id");

    ML_QUERY_EXEC(query);

    while(query.next()) {
        RawiInfo *rawi = new RawiInfo();
        rawi->id = query.value(0).toInt();
        rawi->name = query.value(1).toString();

        m_rowat[rawi->id] = rawi;
    }
}

void TarajemRowatManager::clear()
{
    qDeleteAll(m_rowat);
    m_rowat.clear();
}

void TarajemRowatManager::reloadModels()
{
    clear();
    loadModels();
}

QStandardItemModel *TarajemRowatManager::getRowatModel()
{
    QStandardItemModel *model = new QStandardItemModel();

    model->setHorizontalHeaderLabels(QStringList() << tr("الرواة"));

    foreach(RawiInfo *rawi, m_rowat.values()) {
        QStandardItem *item = new QStandardItem();
        item->setText(Utils::abbreviate(rawi->name, 100));
        item->setToolTip(rawi->name);
        item->setData(rawi->id, ItemRole::authorIdRole);

        model->appendRow(item);
    }

    return model;
}

RawiInfo *TarajemRowatManager::getRawiInfo(int rawiID)
{
    RawiInfo *rawi = m_rowat.value(rawiID);

    if(rawi && m_rowatFullInfo.contains(rawiID))
        return rawi;

    QSqlQuery query(m_db);
    query.prepare("SELECT id, name, laqab, birth_year, birth, death_year, death, "
                  "tabaqa, rowat, rotba_hafed, rotba_zahabi, sheok, talamid, tarejama "
                  "FROM rowat WHERE id = ?");

    query.bindValue(0, rawiID);

    ML_QUERY_EXEC(query);

    if(query.next()) {
        rawi->name = query.value(1).toString();
        rawi->tabaqa = query.value(7).toString();
        rawi->laqab = query.value(2).toString();
        rawi->rowat = query.value(8).toString();
        rawi->rotba_hafed = query.value(9).toString();
        rawi->rotba_zahabi = query.value(10).toString();

        rawi->birthYear = query.value(3).toInt();
        rawi->birthStr = query.value(4).toString();

        if(rawi->birthStr.isEmpty() && !rawi->unknowBirth())
            rawi->birthStr = Utils::hijriYear(rawi->birthYear);

        rawi->deathYear = query.value(5).toInt();
        rawi->deathStr = query.value(6).toString();

        if(rawi->deathStr.isEmpty() && !rawi->unknowDeath())
            rawi->deathStr = Utils::hijriYear(rawi->deathYear);

        rawi->sheok = query.value(11).toString();
        rawi->talamid = query.value(12).toString();
        rawi->tarejama = query.value(13).toString();

        m_rowat[rawi->id] = rawi;
        m_rowatFullInfo.append(rawi->id);
    }

    return rawi;
}

bool TarajemRowatManager::updateRawi(RawiInfo *rawi)
{
    QSqlQuery query(m_db);

    Utils::QueryBuilder q;
    q.setTableName("rowat");
    q.setQueryType(Utils::QueryBuilder::Update);

    q.addColumn("name", rawi->name);
    q.addColumn("laqab", rawi->laqab);

    q.addColumn("birth_year", rawi->unknowBirth() ? RawiInfo::UnknowYear : rawi->birthYear);
    q.addColumn("birth", rawi->unknowBirth() ? QVariant(QVariant::String) : rawi->birthStr);

    q.addColumn("death_year", rawi->unknowBirth() ? RawiInfo::UnknowYear : rawi->deathYear);
    q.addColumn("death", rawi->unknowBirth() ? QVariant(QVariant::String) : rawi->deathStr);

    q.addColumn("tabaqa", rawi->tabaqa);
    q.addColumn("rowat", rawi->rowat);

    q.addColumn("rotba_hafed", rawi->rotba_hafed);
    q.addColumn("rotba_zahabi", rawi->rotba_zahabi);

    q.addColumn("sheok", rawi->sheok);
    q.addColumn("talamid", rawi->talamid);
    q.addColumn("tarejama", rawi->tarejama);

    q.addWhere("id", rawi->id);

    q.prepare(query);

    if(query.exec()) {
        m_rowat[rawi->id] = rawi; //FIXME: Memory leak
        return true;
    } else {
        LOG_SQL_ERROR(query);
        return false;
    }
}

int TarajemRowatManager::addRawi(RawiInfo *rawi)
{
    QMutexLocker locker(&m_mutex);

    if(!rawi->id)
        rawi->id = getNewRawiID();

    QSqlQuery query(m_db);

    Utils::QueryBuilder q;
    q.setTableName("rowat");
    q.setQueryType(Utils::QueryBuilder::Insert);

    q.addColumn("id", rawi->id);
    q.addColumn("name", rawi->name);
    q.addColumn("laqab", rawi->laqab);

    q.addColumn("birth_year", rawi->unknowBirth() ? RawiInfo::UnknowYear : rawi->birthYear);
    q.addColumn("birth", rawi->unknowBirth() ? QVariant(QVariant::String) : rawi->birthStr);

    q.addColumn("death_year", rawi->unknowBirth() ? RawiInfo::UnknowYear : rawi->deathYear);
    q.addColumn("death", rawi->unknowBirth() ? QVariant(QVariant::String) : rawi->deathStr);

    q.addColumn("tabaqa", rawi->tabaqa);
    q.addColumn("rowat", rawi->rowat);

    q.addColumn("rotba_hafed", rawi->rotba_hafed);
    q.addColumn("rotba_zahabi", rawi->rotba_zahabi);

    q.addColumn("sheok", rawi->sheok);
    q.addColumn("talamid", rawi->talamid);
    q.addColumn("tarejama", rawi->tarejama);

    q.prepare(query);

    if(query.exec())
        m_rowat[rawi->id] = rawi;
    else
        LOG_SQL_ERROR(query);

    return rawi->id;
}

bool TarajemRowatManager::removeRawi(int rawiID)
{
    m_query.prepare("DELETE FROM rowat WHERE id = ?");
    m_query.bindValue(0, rawiID);

    if(m_query.exec()) {
        m_rowat.remove(rawiID); //FIXME: memory leak
        return true;
    } else {
        LOG_SQL_ERROR(m_query);
        return false;
    }
}

int TarajemRowatManager::getNewRawiID()
{
    int rawiID = 0;
    do {
        rawiID = Utils::randInt(11111, 99999);
    } while(m_rowat.contains(rawiID));

    return rawiID;
}
