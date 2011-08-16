#include "shamelamanager.h"
#include "common.h"
#include <qvariant.h>
#include <qsqlquery.h>
#include <qsqlerror.h>
#include <qstandarditemmodel.h>

ShamelaManager::ShamelaManager(ShamelaInfo *info)
{
    m_info = info;
    m_haveBookFilter = false;
}

ShamelaManager::~ShamelaManager()
{
    close();
}

void ShamelaManager::openIndexDB()
{
    if(!m_indexDB.isOpen()) {
        QString book = m_library->booksIndexPath();

        m_indexDB = QSqlDatabase::addDatabase("QSQLITE", "indexDb_");
        m_indexDB.setDatabaseName(book);

        if (!m_indexDB.open()) {
            DB_OPEN_ERROR(book);
        }

        m_indexQuery = new QSqlQuery(m_indexDB);
    }
}

void ShamelaManager::openShamelaDB()
{
    if(!m_shamelaDB.isOpen()) {
        QString book = m_info->shamelaMainDbPath();

        m_shamelaDB = QSqlDatabase::addDatabase("QODBC", "shamelaBookDb_");
        QString mdbpath = QString("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=%1").arg(book);
        m_shamelaDB.setDatabaseName(mdbpath);

        if (!m_shamelaDB.open()) {
            DB_OPEN_ERROR(book);
        }

        m_shamelaQuery = new QSqlQuery(m_shamelaDB);
    }
}

void ShamelaManager::openShamelaSpecialDB()
{
    if(!m_shamelaSpecialDB.isOpen()) {
        QString book = m_info->shamelaSpecialDbPath();

        m_shamelaSpecialDB = QSqlDatabase::addDatabase("QODBC", "shamelaSpecialDb_");
        QString mdbpath = QString("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=%1").arg(book);
        m_shamelaSpecialDB.setDatabaseName(mdbpath);

        if (!m_shamelaSpecialDB.open()) {
            DB_OPEN_ERROR(book);
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
    }

    if(m_shamelaSpecialDB.isOpen()) {
        delete m_shamelaSpecialQuery;
        m_shamelaSpecialDB.close();
    }
}

int ShamelaManager::getBooksCount()
{
    openShamelaDB();

    int count=-1;
    m_shamelaQuery->exec("SELECT COUNT(*) FROM 0bok");

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

    m_shamelaSpecialQuery->exec("SELECT COUNT(*) FROM Auth");

    if(m_shamelaSpecialQuery->next()) {
        return m_shamelaSpecialQuery->value(0).toInt();
    }

    return -1;
}

int ShamelaManager::getCatCount()
{
    openShamelaDB();

    m_shamelaQuery->exec("SELECT COUNT(*) FROM 0cat");

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
    specialQuery.exec();

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

    m_shamelaQuery->exec("SELECT id, name, catord, Lvl FROM 0cat ORDER BY catord");
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

    m_shamelaSpecialQuery->exec("SELECT authid, auth, Lng, AD, inf FROM Auth");
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

    m_shamelaQuery->exec("SELECT bkid, bk, cat, betaka, authno, auth, Archive, TafseerNam FROM 0bok ORDER BY Archive");
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

void ShamelaManager::addCatMap(int shamelaID, int libID)
{
    m_catMap.insert(shamelaID, libID);
}

void ShamelaManager::addAuthorMap(int shamelaID, int libID)
{
    m_authorsMap.insert(shamelaID, libID);
}

void ShamelaManager::addBookMap(int shamelaID, int libID)
{
    QMutexLocker locker(&m_mutex);

    m_booksMap.insert(shamelaID, libID);
}

int ShamelaManager::mapShamelaToLibCat(int shamelaID)
{
    return m_catMap.value(shamelaID, 0);
}

int ShamelaManager::mapShamelaToLibAuthor(int shamelaID)
{
    return m_authorsMap.value(shamelaID, 0);
}

int ShamelaManager::mapShamelaToLibBook(int shamelaID)
{
    return m_booksMap.value(shamelaID, 0);
}

int ShamelaManager::getBookShareeh(int shamelaID)
{
    openShamelaSpecialDB();
    QSqlQuery specialQuery(m_shamelaSpecialDB);

    specialQuery.prepare("SELECT Sharh FROM oShrooh WHERE Matn = ?");
    specialQuery.bindValue(0, shamelaID);
    specialQuery.exec();

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
    specialQuery.exec();

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
            SQL_ERROR(specialQuery.lastError().text());

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
