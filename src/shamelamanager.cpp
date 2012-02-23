#include "shamelamanager.h"
#include "utils.h"
#include "modelenums.h"
#include "bookeditor.h"

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
    if(!m_libraryManager.isOpen()) {
        QString book = m_library->booksIndexPath();

        m_libraryManager = QSqlDatabase::addDatabase("QSQLITE", "indexDb_");
        m_libraryManager.setDatabaseName(book);

        if (!m_libraryManager.open()) {
            LOG_DB_ERROR(m_libraryManager);
        }

        m_indexQuery = new QSqlQuery(m_libraryManager);
    }
}

void ShamelaManager::openShamelaDB()
{
    if(!m_shamelaDB.isOpen()) {
        QString book = m_info->shamelaMainDbPath();

#ifdef USE_MDBTOOLS
        MdbConverter mdb(true);
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
        MdbConverter mdb(true);
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
    if(m_libraryManager.isOpen()) {
        delete m_indexQuery;
        m_libraryManager.close();
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

QStandardItemModel *ShamelaManager::getBooksListModel()
{
    openShamelaDB();

    QStandardItemModel *model = new QStandardItemModel();

    QSqlQuery query(m_shamelaDB);
    if(!query.exec(QString("SELECT id, name, Lvl FROM %1 ORDER BY catord").arg(mdbTable("0cat"))))
        LOG_SQL_ERROR(query);

    while(query.next()) {
        QStandardItem *item = new QStandardItem();
        item->setText(query.value(1).toString());
        item->setData(query.value(0).toInt(), ItemRole::idRole);
        item->setData(ItemType::CategorieItem, ItemRole::typeRole);

        item->setCheckable(true);
        item->setCheckState(Qt::Unchecked);

        model->appendRow(item);

        booksCat(item, query.value(0).toInt());
    }

    return model;
}

void ShamelaManager::booksCat(QStandardItem *parentNode, int catID)
{
    openShamelaDB();
    QSqlQuery query(m_shamelaDB);

    query.prepare(QString("SELECT bkid, bk, cat, betaka, authno, auth, Archive "
                          "FROM %1 WHERE cat = ? ORDER BY bkid").arg(mdbTable("0bok")));
    query.bindValue(0, catID);
    if(!query.exec())
        LOG_SQL_ERROR(query);

    while(query.next()) {
        QStandardItem *item = new QStandardItem();
        item->setText(query.value(1).toString());
        item->setData(query.value(0).toInt(), ItemRole::idRole);
        item->setData(ItemType::BookItem, ItemRole::typeRole);

        item->setCheckable(true);
        item->setCheckState(Qt::Unchecked);

        parentNode->appendRow(item);
    }
}

int ShamelaManager::getBooksCount()
{
    openShamelaDB();

    int count=-1;

    if(!m_shamelaQuery->exec(QString("SELECT COUNT(*) FROM %1").arg(mdbTable("0bok"))))
        LOG_SQL_P_ERROR(m_shamelaQuery);

    if(m_shamelaQuery->next()) {
        count = m_shamelaQuery->value(0).toInt();
    }

    if(m_haveBookFilter) {
        count = m_accepted.count();
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

ShamelaAuthorInfo *ShamelaManager::getAuthorInfo(int id)
{
    openShamelaSpecialDB();
    QSqlQuery specialQuery(m_shamelaSpecialDB);

    specialQuery.prepare("SELECT authid, auth, Lng, AD, inf FROM Auth WHERE authid = ?");
    specialQuery.bindValue(0, id);
    if(!specialQuery.exec())
        LOG_SQL_ERROR(specialQuery);

    if(specialQuery.next()) {
        return new ShamelaAuthorInfo(specialQuery.value(0).toInt(),
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

    if(!m_shamelaQuery->exec(QString("SELECT id, name, catord, Lvl FROM %1 ORDER BY catord")
                             .arg(mdbTable("0cat"))))
        LOG_SQL_P_ERROR(m_shamelaQuery);
}

ShamelaCategorieInfo *ShamelaManager::nextCat()
{
    if(m_shamelaQuery->next()) {
        return new ShamelaCategorieInfo(m_shamelaQuery->value(0).toInt(),
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

ShamelaAuthorInfo *ShamelaManager::nextAuthor()
{
    if(m_shamelaSpecialQuery->next()) {
        return new ShamelaAuthorInfo(m_shamelaSpecialQuery->value(0).toInt(),
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

    if(!m_shamelaQuery->exec(QString("SELECT bkid, bk, cat, betaka, authno, auth, Archive, TafseerNam FROM %1 ORDER BY Archive")
                             .arg(mdbTable("0bok"))))
        LOG_SQL_P_ERROR(m_shamelaQuery);
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
        if(m_accepted.contains(bid)) {
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

QList<int> ShamelaManager::getBookShorooh(int shamelaID)
{
    openShamelaSpecialDB();
    QSqlQuery specialQuery(m_shamelaSpecialDB);
    QSet<int> ret;

    specialQuery.prepare("SELECT Sharh FROM oShrooh WHERE Matn = ?");
    specialQuery.bindValue(0, shamelaID);
    if(!specialQuery.exec())
        LOG_SQL_ERROR(specialQuery);

    while(specialQuery.next()) {
            ret << specialQuery.value(0).toInt();
    }

    return ret.toList();
}

void ShamelaManager::importShorooh()
{
    QHash<int, int> booksMap = m_mapper->booksMap();
    QHash<int, int>::const_iterator i = booksMap.constBegin();
    QHash<int, int>::const_iterator end = booksMap.constEnd();

    QHash<int, int> bookToNum; // Convert bkid to oNum
    QHash<int, int> numToBook; // Convert oNum to bkid
    QSqlQuery query(m_shamelaDB);
    query.exec("SELECT bkid, oNum FROM " + mdbTable("0bok"));
    while(query.next()) {
        int bid = query.value(0).toInt();
        int oNum = query.value(1).toInt();

        if(!m_haveBookFilter || m_accepted.contains(bid)) {
            bookToNum.insert(bid, oNum);
            numToBook.insert(oNum, bid);
        }
    }

    while (i != end) {
        BookEditor *editor=0;
        int shaBookId = i.key();
        int bookID = i.value();

        // Get linked shorooh
        QList<int> shorooh = getBookShorooh(bookToNum[shaBookId]);
        if(!shorooh.isEmpty()) {
            qDebug("Book %d has %d shareeh", shaBookId, shorooh.size());
            // Link with each shareeh
            foreach(int bookNum, shorooh) {
                // Convert oNum to bkid
                int bkid = numToBook[bookNum];
                if(!bkid) {
                    qDebug("Can't find bkid for book with oNum %d", bookNum);
                    continue;
                }

                if(m_haveBookFilter && !m_accepted.contains(bkid)) {
                    qDebug("Shareeh %d not imported from shamela", bkid);
                    continue;
                }

                // Convert bkid to this library book id
                int shareehLibID = booksMap[bkid];
                if(!shareehLibID) {
                    qDebug("Can't map book %d", bkid);
                    continue;
                }

                // Open editor in the first time
                if(!editor) {
                    editor = new BookEditor();
                    if(!editor->open(bookID)) {
                        delete editor;
                        editor = 0;
                        continue;
                    }

                    editor->unZip();
                }

                qDebug("Link %d with %d", bookID, shareehLibID);

                // Start linking...
                QSqlQuery specialQuery(m_shamelaSpecialDB);
                specialQuery.prepare("SELECT MatnId, SharhId FROM oShr WHERE Matn = ? AND Sharh = ?");
                specialQuery.bindValue(0, bookToNum[shaBookId]); // We should use oNum
                specialQuery.bindValue(1, bookNum);
                if(specialQuery.exec()) {
                    QList<int> linked; // To remove duplicated entries
                    while(specialQuery.next()) {
                        if(!linked.contains(specialQuery.value(1).toInt())) {
                            editor->addPageLink(specialQuery.value(0).toInt(),
                                                shareehLibID,
                                                specialQuery.value(1).toInt());
                            linked<<specialQuery.value(1).toInt();
                        }
                    }
                } else {
                    LOG_SQL_ERROR(specialQuery);
                    continue;
                }
            }

            if(editor) {
                editor->saveDom();
                editor->zip();
                editor->save();
                editor->removeTemp();

                delete editor;
                editor = 0;
            }
        }

        ++i;
    }
}

QString ShamelaManager::mdbTable(QString table)
{
#ifdef USE_MDBTOOLS
    if(table.at(0).isDigit()) {
        table[0] = '_';
    }
#endif
    return table;

}
