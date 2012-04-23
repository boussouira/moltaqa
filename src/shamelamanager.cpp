#include "shamelamanager.h"
#include "utils.h"
#include "modelenums.h"
#include "bookeditor.h"
#include "mainwindow.h"
#include "librarymanager.h"
#include "librarybookmanager.h"
#include "librarybook.h"

#ifdef USE_MDBTOOLS
#include"mdbconverter.h"
#endif

#include <qvariant.h>
#include <qsqlquery.h>
#include <qsqlerror.h>
#include <qstandarditemmodel.h>

ShamelaManager::ShamelaManager(ShamelaInfo *info) :
     m_info(info),
     m_mapper(new ShamelaMapper()),
     m_shamelaQuery(0),
     m_shamelaSpecialQuery(0)
{
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

void ShamelaManager::openShamelaDB()
{
    QMutexLocker locker(&m_mutex);

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
    QMutexLocker locker(&m_mutex);

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
    m_remover.removeDatabase(m_shamelaDB);
    m_remover.removeDatabase(m_shamelaSpecialDB);

    ML_DELETE_CHECK(m_shamelaQuery);
    ML_DELETE_CHECK(m_shamelaSpecialQuery);
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

ShoortsList ShamelaManager::getBookShoorts(int bookID)
{
    openShamelaSpecialDB();

    ShoortsList shoorts;
    QSqlQuery specialQuery(m_shamelaSpecialDB);

    if(!specialQuery.exec(QString("SELECT Ramz, Nass FROM shorts WHERE Bk = '%1'").arg(bookID)))
        LOG_SQL_ERROR(specialQuery);

    while(specialQuery.next()) {
        QPair<QString, QString> pair;
        pair.first = specialQuery.value(0).toString();
        pair.second = specialQuery.value(1).toString();

        shoorts.append(pair);
    }

    return shoorts;
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

    if(!m_shamelaQuery->exec(QString("SELECT bkid, bk, cat, betaka, inf, authno, auth, Archive, TafseerNam "
                                     "FROM %1 ORDER BY Archive")
                             .arg(mdbTable("0bok")))) {
        LOG_SQL_P_ERROR(m_shamelaQuery);
    }
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
                                       m_shamelaQuery->value(4).toString(),
                                       m_shamelaQuery->value(7).toInt(),
                                       m_shamelaQuery->value(2).toInt(),
                                       m_shamelaQuery->value(5).toInt(),
                                       m_shamelaQuery->value(6).toString(),
                                       m_shamelaQuery->value(8).toString());
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
                                       m_shamelaQuery->value(4).toString(),
                                       m_shamelaQuery->value(7).toInt(),
                                       m_shamelaQuery->value(2).toInt(),
                                       m_shamelaQuery->value(5).toInt(),
                                       m_shamelaQuery->value(6).toString(),
                                       m_shamelaQuery->value(8).toString());
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
        int bkid = query.value(0).toInt();
        int oNum = query.value(1).toInt();

        if(!m_haveBookFilter || m_accepted.contains(bkid)) {
            bookToNum.insert(bkid, oNum);

            if(!oNum)
                oNum = bkid;
            numToBook.insert(oNum, bkid);
        }
    }

    LibraryBookManager *bookManager = LibraryManager::instance()->bookManager();

    while (i != end) {
        BookEditor *editor=0;
        int mateen_bkId = i.key();
        int mateen_oNum = bookToNum[mateen_bkId];
        int mateen_libID = i.value();

        // Get linked shorooh
        QList<int> shorooh = getBookShorooh(mateen_oNum);
        if(!shorooh.isEmpty()) {
            qDebug("ShamelaManager::importShorooh Book %d has %d shorooh", mateen_bkId, shorooh.size());
            // Link with each shareeh
            foreach(int shareeh_oNum, shorooh) {
                // Convert oNum to bkid
                int shareeh_bkid = numToBook[shareeh_oNum];
                if(!shareeh_bkid) {
                    qDebug("ShamelaManager::importShorooh Can't find bkid for book with oNum %d", shareeh_oNum);
                    continue;
                }

                if(m_haveBookFilter && !m_accepted.contains(shareeh_bkid)) {
                    qDebug("ShamelaManager::importShorooh Shareeh %d not imported from shamela", shareeh_bkid);
                    continue;
                }

                // Convert bkid to this library book id
                int shareeh_LibID = m_mapper->mapFromShamelaBook(shareeh_bkid);
                if(!shareeh_LibID) {
                    qDebug("ShamelaManager::importShorooh Can't map book %d", shareeh_bkid);
                    continue;
                }

                // Open editor in the first time
                if(!editor) {
                    editor = new BookEditor();
                    if(!editor->open(mateen_libID)) {
                        delete editor;
                        editor = 0;
                        continue;
                    }

                    editor->unZip();

                    qApp->processEvents();
                }

                LibraryBookPtr mi = bookManager->getLibraryBook(mateen_libID);
                LibraryBookPtr si = bookManager->getLibraryBook(shareeh_LibID);

                qDebug() << "ShamelaManager::importShorooh"
                         << "Mateen:" <<(mi ? mi->title : "????")
                         << "Shareeh:" << (si ? si->title : "????");

                // Start linking...
                QSqlQuery specialQuery(m_shamelaSpecialDB);
                specialQuery.prepare("SELECT MatnId, SharhId FROM oShr WHERE Matn = ? AND Sharh = ?");
                specialQuery.bindValue(0, mateen_oNum); // We should use oNum
                specialQuery.bindValue(1, shareeh_oNum);
                if(specialQuery.exec()) {
                    QList<int> linked; // To remove duplicated entries
                    while(specialQuery.next()) {
                        if(!linked.contains(specialQuery.value(1).toInt())) {
                            editor->addPageLink(specialQuery.value(0).toInt(),
                                                shareeh_LibID,
                                                specialQuery.value(1).toInt());
                            linked << specialQuery.value(1).toInt();
                        }
                    }
                } else {
                    LOG_SQL_ERROR(specialQuery);
                    continue;
                }

                qApp->processEvents();
            }

            if(editor) {
                editor->saveDom();
                editor->zip();
                editor->save();
                editor->removeTemp();

                qApp->processEvents();

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
