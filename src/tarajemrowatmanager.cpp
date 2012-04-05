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

    ML_QUERY_EXEC(query);

    QStandardItemModel *model = new QStandardItemModel();
    model->setHorizontalHeaderLabels(QStringList() << tr("الرواة"));

    while(query.next()) {
        QStandardItem *item = new QStandardItem();
        item->setText(Utils::abbreviate(query.value(1).toString(), 100));
        item->setToolTip(query.value(1).toString());
        item->setData(query.value(0).toInt(), ItemRole::authorIdRole);

        model->appendRow(item);
    }

    return model;
}

RawiInfoPtr TarajemRowatManager::getRawiInfo(int rawiID)
{
    RawiInfoPtr rawi = m_rowat.value(rawiID);
    if(rawi)
        return rawi;

    QSqlQuery query(m_db);
    query.prepare("SELECT id, name, laqab, birth_year, birth, death_year, death, "
                  "tabaqa, rowat, rotba_hafed, rotba_zahabi, sheok, talamid, tarejama "
                  "FROM rowat WHERE id = ?");

    query.bindValue(0, rawiID);

    ML_QUERY_EXEC(query);

    if(query.next()) {
        rawi = RawiInfoPtr(new RawiInfo());
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
            rawi->birthStr = Utils::hijriYear(rawi->birthYear);

        if(!query.value(5).isNull()) {
            rawi->deathYear = query.value(5).toInt();
            rawi->deathStr = query.value(6).toString();
        }

        if(rawi->deathStr.isEmpty() && !rawi->unknowDeath())
            rawi->deathStr = Utils::hijriYear(rawi->deathYear);

        rawi->sheok = query.value(11).toString();
        rawi->talamid = query.value(12).toString();
        rawi->tarejama = query.value(13).toString();

        m_rowat[rawi->id] = rawi;
    }

    return rawi;
}

bool TarajemRowatManager::updateRawi(RawiInfoPtr rawi)
{
    QSqlQuery query(m_db);

    Utils::QueryBuilder q;
    q.setTableName("rowat");
    q.setQueryType(Utils::QueryBuilder::Update);

    q.addColumn("name", rawi->name);
    q.addColumn("laqab", rawi->laqab);

    q.addColumn("birth_year", rawi->unknowBirth() ? QVariant(QVariant::Int) : rawi->birthYear);
    q.addColumn("birth", rawi->unknowBirth() ? QVariant(QVariant::String) : rawi->birthStr);

    q.addColumn("death_year", rawi->unknowBirth() ? QVariant(QVariant::Int) : rawi->deathYear);
    q.addColumn("death", rawi->unknowBirth() ? QVariant(QVariant::String) : rawi->deathStr);

    q.addColumn("tabaqa", rawi->tabaqa);
    q.addColumn("rowat", rawi->rowat);

    q.addColumn("rotba_hafed", rawi->rotba_hafed);
    q.addColumn("rotba_zahabi", rawi->rotba_zahabi);

    q.addColumn("sheok", rawi->sheok);
    q.addColumn("talamid", rawi->talamid);
    q.addColumn("tarejama", rawi->tarejama);

    q.addWhere("id", rawi->id);

    ML_ASSERT_RET(q.exec(query), false);

    m_rowat[rawi->id] = rawi; //FIXME: Memory leak
    return true;
}

int TarajemRowatManager::addRawi(RawiInfoPtr rawi)
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

    q.addColumn("birth_year", rawi->unknowBirth() ? QVariant(QVariant::Int) : rawi->birthYear);
    q.addColumn("birth", rawi->unknowBirth() ? QVariant(QVariant::String) : rawi->birthStr);

    q.addColumn("death_year", rawi->unknowBirth() ? QVariant(QVariant::Int) : rawi->deathYear);
    q.addColumn("death", rawi->unknowBirth() ? QVariant(QVariant::String) : rawi->deathStr);

    q.addColumn("tabaqa", rawi->tabaqa);
    q.addColumn("rowat", rawi->rowat);

    q.addColumn("rotba_hafed", rawi->rotba_hafed);
    q.addColumn("rotba_zahabi", rawi->rotba_zahabi);

    q.addColumn("sheok", rawi->sheok);
    q.addColumn("talamid", rawi->talamid);
    q.addColumn("tarejama", rawi->tarejama);

    ML_ASSERT_RET(q.exec(query), 0);

    m_rowat[rawi->id] = rawi;

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
