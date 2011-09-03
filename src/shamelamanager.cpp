#include "shamelamanager.h"
#include "utils.h"

#ifdef USE_MDBTOOLS
#include"mdbconverter.h"
#endif

#include <qvariant.h>
#include <qsqlquery.h>
#include <qsqlerror.h>
#include <qstandarditemmodel.h>

ShamelaManager::ShamelaManager(ShamelaInfo *info)
{
    m_mapper = new ShamelaMapper();
    m_info = info;
    m_haveBookFilter = false;
}

ShamelaManager::~ShamelaManager()
{
    close();
    delete m_mapper;
}

ShamelaMapper *ShamelaManager::mapper()
{
    return m_mapper;
}

void ShamelaManager::openIndexDB()
{
    if(!m_indexDB.isOpen()) {
        QString book = m_library->booksIndexPath();

        m_indexDB = QSqlDatabase::addDatabase("QSQLITE", "indexDb_");
        m_indexDB.setDatabaseName(book);

        if (!m_indexDB.open()) {
            LOG_DB_ERROR(m_indexDB);
        }

        m_indexQuery = new QSqlQuery(m_indexDB);
    }
}

void ShamelaManager::openShamelaDB()
{
    if(!m_shamelaDB.isOpen()) {
        QString book = m_info->shamelaMainDbPath();

#ifdef USE_MDBTOOLS
    MdbConverter mdb;
    m_tempShamelaDB = mdb.exportFromMdb(book);

    m_shamelaDB = QSqlDatabase::addDatabase("QSQLITE", "shamelaBookDb_mdb_");
    m_shamelaDB.setDatabaseName(m_tempShamelaDB);
#else
        m_shamelaDB = QSqlDatabase::addDatabase("QODBC", "shamelaBookDb_");
        QString mdbpath = QString("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=%1").arg(book);
        m_shamelaDB.setDatabaseName(mdbpath);
#endif

        if (!m_shamelaDB.open()) {
            LOG_DB_ERROR(m_shamelaDB);
        }

        m_shamelaQuery = new QSqlQuery(m_shamelaDB);
    }
}

void ShamelaManager::openShamelaSpecialDB()
{
    if(!m_shamelaSpecialDB.isOpen()) {
        QString book = m_info->shamelaSpecialDbPath();

#ifdef USE_MDBTOOLS
        MdbConverter mdb;
        m_tempshamelaSpecialDB = mdb.exportFromMdb(book);

        m_shamelaSpecialDB = QSqlDatabase::addDatabase("QSQLITE", "shamelaSpecialDb_mdb_");
        m_shamelaSpecialDB.setDatabaseName(m_tempshamelaSpecialDB);
#else
        m_shamelaSpecialDB = QSqlDatabase::addDatabase("QODBC", "shamelaSpecialDb_");
        QString mdbpath = QString("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=%1").arg(book);
        m_shamelaSpecialDB.setDatabaseName(mdbpath);
#endif

        if (!m_shamelaSpecialDB.open()) {
            LOG_DB_ERROR(m_shamelaSpecialDB);
        }

        m_shamelaSpecialQuery = new QSqlQuery(m_shamelaSpecialDB);
    }
}

void ShamelaManager::close()
{
    if(m_indexDB.isOpen()) {
        delete m_indexQuery;
        m_indexDB.close();
    }

    if(m_shamelaDB.isOpen()) {
        delete m_shamelaQuery;
        m_shamelaDB.close();

#ifdef USE_MDBTOOLS
        QFile::remove(m_tempShamelaDB);
#endif
    }

    if(m_shamelaSpecialDB.isOpen()) {
        delete m_shamelaSpecialQuery;
        m_shamelaSpecialDB.close();

#ifdef USE_MDBTOOLS
        QFile::remove(m_tempshamelaSpecialDB);
#endif
    }
}

int ShamelaManager::getBooksCount()
{
    openShamelaDB();

    int count=-1;

#ifdef USE_MDBTOOLS
    if(!m_shamelaQuery->exec("SELECT COUNT(*) FROM _bok"))
        LOG_SQL_P_ERROR(m_shamelaQuery);
#else
    if(!m_shamelaQuery->exec("SELECT COUNT(*) FROM 0bok"))
        LOG_SQL_P_ERROR(m_shamelaQuery);
#endif

    if(m_shamelaQuery->next()) {
        count = m_shamelaQuery->value(0).toInt();
    }

    if(m_haveBookFilter) {
        if(!m_accepted.isEmpty())
            count = m_accepted.count();
        else
            count -= m_rejected.count();
    }

    return count;
}

int ShamelaManager::getAuthorsCount()
{
    openShamelaSpecialDB();

    if(!m_shamelaSpecialQuery->exec("SELECT COUNT(*) FROM Auth"))
        LOG_SQL_P_ERROR(m_shamelaSpecialQuery);

    if(m_shamelaSpecialQuery->next()) {
        return m_shamelaSpecialQuery->value(0).toInt();
    }

    return -1;
}

int ShamelaManager::getCatCount()
{
    openShamelaDB();

#ifdef USE_MDBTOOLS
    if(!m_shamelaQuery->exec("SELECT COUNT(*) FROM _cat"))
        LOG_SQL_P_ERROR(m_shamelaQuery);
#else
    if(!m_shamelaQuery->exec("SELECT COUNT(*) FROM 0cat"))
        LOG_SQL_P_ERROR(m_shamelaQuery);
#endif

    if(m_shamelaQuery->next()) {
        return m_shamelaQuery->value(0).toInt();
    }

    return -1;
}

AuthorInfo *ShamelaManager::getAuthorInfo(int id)
{
    openShamelaSpecialDB();
    QSqlQuery specialQuery(m_shamelaSpecialDB);

    specialQuery.prepare("SELECT authid, auth, Lng, AD, inf FROM Auth WHERE authid = ?");
    specialQuery.bindValue(0, id);
    if(!specialQuery.exec())
        LOG_SQL_ERROR(specialQuery);

    if(specialQuery.next()) {
        return new AuthorInfo(specialQuery.value(0).toInt(),
                              specialQuery.value(3).toInt(),
                              specialQuery.value(1).toString(),
                              specialQuery.value(2).toString(),
                              specialQuery.value(4).toString());
    } else {
        return 0;
    }
}

void ShamelaManager::selectCats()
{
    openShamelaDB();

#ifdef USE_MDBTOOLS
    if(!m_shamelaQuery->exec("SELECT id, name, catord, Lvl FROM _cat ORDER BY catord"))
        LOG_SQL_P_ERROR(m_shamelaQuery);
#else
    if(!m_shamelaQuery->exec("SELECT id, name, catord, Lvl FROM 0cat ORDER BY catord"))
        LOG_SQL_P_ERROR(m_shamelaQuery);
#endif
}

CategorieInfo *ShamelaManager::nextCat()
{
    if(m_shamelaQuery->next()) {
        return new CategorieInfo(m_shamelaQuery->value(0).toInt(),
                                 m_shamelaQuery->value(1).toString(),
                                 m_shamelaQuery->value(2).toInt(),
                                 m_shamelaQuery->value(3).toInt());
    } else {
        return 0;
    }
}

void ShamelaManager::selectAuthors()
{
    openShamelaSpecialDB();

    if(!m_shamelaSpecialQuery->exec("SELECT authid, auth, Lng, AD, inf FROM Auth"))
        LOG_SQL_P_ERROR(m_shamelaSpecialQuery);
}

AuthorInfo *ShamelaManager::nextAuthor()
{
    if(m_shamelaSpecialQuery->next()) {
        return new AuthorInfo(m_shamelaSpecialQuery->value(0).toInt(),
                              m_shamelaSpecialQuery->value(3).toInt(),
                              m_shamelaSpecialQuery->value(1).toString(),
                              m_shamelaSpecialQuery->value(2).toString(),
                              m_shamelaSpecialQuery->value(4).toString());
    } else {
        return 0;
    }
}

void ShamelaManager::selectBooks()
{
    openShamelaDB();

#ifdef USE_MDBTOOLS
    if(!m_shamelaQuery->exec("SELECT bkid, bk, cat, betaka, authno, auth, Archive, TafseerNam FROM _bok ORDER BY Archive"))
        LOG_SQL_P_ERROR(m_shamelaQuery);
#else
    if(!m_shamelaQuery->exec("SELECT bkid, bk, cat, betaka, authno, auth, Archive, TafseerNam FROM 0bok ORDER BY Archive"))
        LOG_SQL_P_ERROR(m_shamelaQuery);
#endif
}

ShamelaBookInfo *ShamelaManager::nextBook()
{
    QMutexLocker locker(&m_mutex);

    if(m_haveBookFilter) {
        return nextFiltredBook();
    } else {
        if(m_shamelaQuery->next()) {
            return new ShamelaBookInfo(m_shamelaQuery->value(0).toInt(),
                                       m_shamelaQuery->value(1).toString(),
                                       m_shamelaQuery->value(3).toString(),
                                       m_shamelaQuery->value(6).toInt(),
                                       m_shamelaQuery->value(2).toInt(),
                                       m_shamelaQuery->value(4).toInt(),
                                       m_shamelaQuery->value(5).toString(),
                                       m_shamelaQuery->value(7).toString());
        } else {
            return 0;
        }
    }
}

ShamelaBookInfo *ShamelaManager::nextFiltredBook()
{
    while(m_shamelaQuery->next()) {
        int bid = m_shamelaQuery->value(0).toInt();
        if(m_accepted.contains(bid) || (!m_rejected.contains(bid) && !m_rejected.isEmpty())) {
            return new ShamelaBookInfo(m_shamelaQuery->value(0).toInt(),
                                       m_shamelaQuery->value(1).toString(),
                                       m_shamelaQuery->value(3).toString(),
                                       m_shamelaQuery->value(6).toInt(),
                                       m_shamelaQuery->value(2).toInt(),
                                       m_shamelaQuery->value(4).toInt(),
                                       m_shamelaQuery->value(5).toString(),
                                       m_shamelaQuery->value(7).toString());
        }
    }

    return 0;
}

void ShamelaManager::setFilterBooks(bool filter)
{
    m_haveBookFilter = filter;
}

void ShamelaManager::setAcceptedBooks(QList<int> accepted)
{
    m_accepted = accepted;
}

void ShamelaManager::setRejectedBooks(QList<int> rejected)
{
    m_rejected = rejected;
}

int ShamelaManager::getBookShareeh(int shamelaID)
{
    openShamelaSpecialDB();
    QSqlQuery specialQuery(m_shamelaSpecialDB);

    specialQuery.prepare("SELECT Sharh FROM oShrooh WHERE Matn = ?");
    specialQuery.bindValue(0, shamelaID);
    if(!specialQuery.exec())
        LOG_SQL_ERROR(specialQuery);

    if(specialQuery.next()) {
        return specialQuery.value(0).toInt();
    }

    return 0;
}

int ShamelaManager::getBookMateen(int shamelaID)
{
    openShamelaSpecialDB();
    QSqlQuery specialQuery(m_shamelaSpecialDB);

    specialQuery.prepare("SELECT Matn FROM oShrooh WHERE Sharh = ?");
    specialQuery.bindValue(0, shamelaID);
    if(!specialQuery.exec())
        LOG_SQL_ERROR(specialQuery);

    if(specialQuery.next()) {
        return specialQuery.value(0).toInt();
    }

    return 0;
}

QList<ShamelaShareehInfo *> ShamelaManager::getShareehInfo(int mateen, int shareeh)
{
    openShamelaSpecialDB();
    QSqlQuery specialQuery(m_shamelaSpecialDB);
    QList<ShamelaShareehInfo *> list;

    if(!m_addedShorooh.contains(shareeh)) {
        specialQuery.prepare("SELECT MatnId, SharhId FROM oShr WHERE Matn = ? AND Sharh = ?");
        specialQuery.bindValue(0, mateen);
        specialQuery.bindValue(1, shareeh);
        if(!specialQuery.exec())
            LOG_SQL_ERROR(specialQuery);

        while(specialQuery.next()) {
            list.append(new ShamelaShareehInfo(mateen,
                                               specialQuery.value(0).toInt(),
                                               shareeh,
                                               specialQuery.value(1).toInt()));
        }

        m_addedShorooh.append(shareeh);
        qDebug("New shareeh %d", shareeh);
    } else {
        qDebug("shareeh %d exist", shareeh);
    }

    return list;
}
