#include "librarycreator.h"
#include "utils.h"
#include "newbookwriter.h"
#include "librarybook.h"
#include "shamelaimportdialog.h"
#include "newquranwriter.h"
#include "mainwindow.h"
#include "libraryenums.h"

#ifdef USE_MDBTOOLS
#include "mdbconverter.h"
#endif

#include <qdebug.h>
#include <qvariant.h>
#include <qfileinfo.h>
#include <qsqlrecord.h>

LibraryCreator::LibraryCreator()
{
    ShamelaImportDialog *importDialog = ShamelaImportDialog::importDialog();

    m_shamelaManager = importDialog->shamelaManager();
    m_shamelaInfo = importDialog->shamelaInfo();
    m_library = importDialog->libraryInfo();
    m_mapper = m_shamelaManager->mapper();

    m_libraryManager = MW->libraryManager();

    m_prevArchive = -1;
    m_threadID = 0;
}

LibraryCreator::~LibraryCreator()
{
    if(m_bookDB.isOpen())
        m_remover.connectionName = m_bookDB.connectionName();
}

void LibraryCreator::openDB()
{
    if(!m_bookDB.isOpen()) {
        m_bookDB = QSqlDatabase::addDatabase("QSQLITE", QString("newBookIndexDB_%1").arg(m_threadID));
        m_bookDB.setDatabaseName(m_library->booksIndexPath());

        if(!m_bookDB.open()) {
            qDebug() << QObject::tr("لم يمكن فتح قاعدة البيانات: %1").arg(m_library->booksIndexPath());
        }

        m_bookQuery = QSqlQuery(m_bookDB);
    }
}

void LibraryCreator::createTables()
{
    Utils::createIndexDB(m_bookQuery);
}

void LibraryCreator::importCats()
{
    m_shamelaManager->selectCats();

    ShamelaCategorieInfo *cat = m_shamelaManager->nextCat();
    while(cat) {
        addCat(cat);

        delete cat;
        cat = m_shamelaManager->nextCat();
    }
}

void LibraryCreator::importAuthors()
{
    m_shamelaManager->selectAuthors();

    ShamelaAuthorInfo *auth = m_shamelaManager->nextAuthor();
    while(auth) {
        addAuthor(auth);

        delete auth;
        auth = m_shamelaManager->nextAuthor();
    }
}

void LibraryCreator::addTafessir(ShamelaBookInfo *tafessir)
{
    m_bookQuery.prepare("INSERT INTO tafassirList (id, book_id, tafessir_name, show_tafessir, tafessir_order) "
                        "VALUES (NULL, ?, ?, ?, ?)");
    m_bookQuery.bindValue(0,  m_mapper->mapFromShamelaBook(tafessir->id));
    m_bookQuery.bindValue(1, tafessir->tafessirName);
    m_bookQuery.bindValue(2, Qt::Checked);
    m_bookQuery.bindValue(3, 0);

    if(m_bookQuery.exec()) {
        qDebug() << "Add tafessir:" << tafessir->tafessirName;
    } else {
        LOG_SQL_ERROR(m_bookQuery);
    }
}

void LibraryCreator::addCat(ShamelaCategorieInfo *cat)
{
    int lastId = 0;

    m_bookQuery.prepare("INSERT INTO catList (id, title, parentID, catOrder) VALUES (NULL, ?, ?, ?)");
    m_bookQuery.bindValue(0, cat->name);
    m_bookQuery.bindValue(1, m_levels.value(cat->level-1, 0));
    m_bookQuery.bindValue(2, 999);

    if(m_bookQuery.exec()) {
        lastId = m_bookQuery.lastInsertId().toInt();
        m_mapper->addCatMap(cat->id, lastId);
        m_catMap.insert(cat->id, lastId);
    } else {
        LOG_SQL_ERROR(m_bookQuery);
    }

    int levelParent = m_levels.value(cat->level, -1);
    if(levelParent == -1) {
        m_levels.insert(cat->level, lastId);
    } else {
            m_levels[cat->level] = lastId;
    }
}

void LibraryCreator::addAuthor(ShamelaAuthorInfo *auth, bool checkExist)
{
    QMutexLocker locker(&m_mutex);

    QSqlQuery bookQuery(m_bookDB);
    if(checkExist) {
        int lid = m_mapper->mapFromShamelaAuthor(auth->id);
        if(lid) {
            // This author is already in the map
            return;
        } else {
            // We look for this author in the index database
            QPair<int, QString> foundAuthor = m_libraryManager->findAuthor(auth->name);

            // If found the author in our database so add it to the map and return
            if(foundAuthor.first) {
                m_mapper->addAuthorMap(auth->id, foundAuthor.first);
                return;
            }
        }
    }

    // Add this author from shamela
    bookQuery.prepare("INSERT INTO authorsList (id, name, full_name, die_year, info) VALUES (NULL, ?, ?, ?, ?)");
    bookQuery.bindValue(0, auth->name);
    bookQuery.bindValue(1, auth->fullName);
    bookQuery.bindValue(2, auth->dieYear);
    bookQuery.bindValue(3, qCompress(auth->info.toUtf8()));

    if(bookQuery.exec())
        m_mapper->addAuthorMap(auth->id, bookQuery.lastInsertId().toInt());
    else
        LOG_SQL_ERROR(bookQuery);
}

void LibraryCreator::addBook(ShamelaBookInfo *book)
{
    Utils::DatabaseRemover remover;
    QString connName(QString("mdb_%1_%2").arg(m_threadID).arg(book->archive));
    QString path = Utils::genBookName(m_library->booksDir(), true);

    NewBookWriter bookWrite;
    bookWrite.setThreadID(m_threadID);
    bookWrite.createNewBook(path);

    QSqlDatabase bookDB;
    if(book->archive && book->archive == m_prevArchive) {
        bookDB = QSqlDatabase::database(connName);
    } else {
        // Remove old connection
        QString prevConnName(QString("mdb_%1_%2").arg(m_threadID).arg(m_prevArchive));
        QSqlDatabase::database(prevConnName, false).close();
        QSqlDatabase::removeDatabase(prevConnName);

#ifdef USE_MDBTOOLS
        MdbConverter::removeConvertedDB(m_tempDB);
        m_tempDB = book->path;

        MdbConverter converter(true);
        QString mdb = converter.exportFromMdb(book->path);

        bookDB = QSqlDatabase::addDatabase("QSQLITE", connName);
        bookDB.setDatabaseName(mdb);
#else
        bookDB = QSqlDatabase::addDatabase("QODBC", connName);
        bookDB.setDatabaseName(QString("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=%1").arg(book->path));
#endif

        m_prevArchive = book->archive;
    }

    if(!bookDB.isOpen()) {
        if (!bookDB.open()) {
            LOG_DB_ERROR(bookDB);
        }
    }

    QSqlQuery query(bookDB);
#ifdef USE_MDBTOOLS
    if(!query.exec(QString("SELECT * FROM %1 LIMIT 1").arg(book->mainTable)))
        LOG_SQL_ERROR(query);
#else
    if(!query.exec(QString("SELECT TOP 1 * FROM %1").arg(book->mainTable)))
        LOG_SQL_ERROR(query);
#endif

    int hnoCol = query.record().indexOf("hno");
    int ayaCol = query.record().indexOf("aya");
    int soraCol = query.record().indexOf("sora");

    bookWrite.startReading();

    if(ayaCol != -1 && soraCol != -1) {
        readTafessirBook(book, query, bookWrite, hnoCol!=-1);
    } else {
        readSimpleBook(book, query, bookWrite, hnoCol!=-1);
    }

    query.prepare(QString("SELECT id, tit, lvl, sub FROM %1 ORDER BY id").arg(book->tocTable));
    if(query.exec()) {
        while(query.next()) {
            bookWrite.addTitle(query.value(1).toString(),
                               query.value(0).toInt(),
                               query.value(2).toInt());
        }
    } else {
        LOG_SQL_ERROR(query);
    }

    bookWrite.endReading();

    if(!book->archive)
        remover.connectionName = connName;

    importBook(book, path);

    if(!getShorooh(book)) {
        getMateen(book);
    }

    if(!book->tafessirName.isEmpty()) {
        addTafessir(book);
    }
}

void LibraryCreator::addQuran()
{
    QString connName(QString("shamela_quran_%1").arg(m_threadID));
    QString path = Utils::genBookName(m_library->booksDir(), true);
    QString tempDB;

    {
        newQuranWriter quranWrite;
        quranWrite.setThreadID(m_threadID);
        quranWrite.createNewBook(path);
#ifdef USE_MDBTOOLS
        MdbConverter mdb(true);
        tempDB = mdb.exportFromMdb(m_shamelaInfo->shamelaSpecialDbPath());

        QSqlDatabase bookDB = QSqlDatabase::addDatabase("QSQLITE", connName);
        bookDB.setDatabaseName(tempDB);
#else
        QSqlDatabase bookDB = QSqlDatabase::addDatabase("QODBC", connName);
        bookDB.setDatabaseName(QString("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=%1")
                               .arg(m_shamelaInfo->shamelaSpecialDbPath()));
#endif

        if (!bookDB.open()) {
            LOG_DB_ERROR(bookDB);
        }

        QSqlQuery query(bookDB);
        quranWrite.startReading();

        query.prepare("SELECT nass, sora, aya, Page FROM Qr ORDER BY id");
        if(query.exec()) {
            while(query.next()) {
                quranWrite.addPage(query.value(0).toString(),
                                   query.value(1).toInt(),
                                   query.value(2).toInt(),
                                   query.value(3).toInt());
            }
        } else {
            LOG_SQL_ERROR(query);
        }

        quranWrite.addSowarInfo();
        quranWrite.endReading();
    }

    QSqlDatabase::removeDatabase(QString("newQuranDB_%1").arg(m_threadID));
    QSqlDatabase::removeDatabase(connName);

#ifdef USE_MDBTOOLS
    QFile::remove(tempDB);
#endif

    importQuran(path);
}

void LibraryCreator::start()
{
    m_bookDB.transaction();
}

void LibraryCreator::done()
{
    if(m_bookDB.commit())
        qDebug("LibraryCreator: Commit...");
    else
        qFatal("Error when committing");
}

void LibraryCreator::importBook(ShamelaBookInfo *book, QString path)
{
    QFileInfo fileInfo(path);

    ShamelaAuthorInfo *auth = m_shamelaManager->getAuthorInfo(book->authorID);
    if(auth) {
        addAuthor(auth, true);
        delete auth;
    }

    m_bookQuery.prepare("INSERT INTO booksList (id, bookID, bookType, bookFlags, bookCat, "
                       "bookDisplayName, bookInfo, authorName, authorID, fileName, bookFolder, indexFlags) "
                       "VALUES(NULL, :book_id, :book_type, :book_flags, :cat_id, :book_name, "
                       ":book_info, :author_name, :author_id, :file_name, :book_folder, :index_flags)");

    m_bookQuery.bindValue(":book_id", 0);
    m_bookQuery.bindValue(":book_type", book->tafessirName.isEmpty() ? LibraryBook::NormalBook : LibraryBook::TafessirBook);
    m_bookQuery.bindValue(":book_flags", 0);
    m_bookQuery.bindValue(":cat_id", m_mapper->mapFromShamelaCat(book->cat));
    m_bookQuery.bindValue(":book_name", book->name);
    m_bookQuery.bindValue(":book_info", book->info);
    m_bookQuery.bindValue(":author_name", book->authName);
    m_bookQuery.bindValue(":author_id", m_mapper->mapFromShamelaAuthor(book->authorID));
    m_bookQuery.bindValue(":file_name", fileInfo.fileName()); // Add file name
    m_bookQuery.bindValue(":book_folder", QVariant(QVariant::String));
    m_bookQuery.bindValue(":index_flags", Enums::NotIndexed);

    if(m_bookQuery.exec()) {
        m_mapper->addBookMap(book->id, m_bookQuery.lastInsertId().toInt());
    } else {
        LOG_SQL_ERROR(m_bookQuery);
    }
}

void LibraryCreator::importQuran(QString path)
{
    QFileInfo fileInfo(path);

    m_bookQuery.prepare("INSERT INTO booksList (id, bookID, bookType, bookFlags, bookCat, "
                        "bookDisplayName, bookInfo, authorName, authorID, fileName, bookFolder, indexFlags) "
                        "VALUES(NULL, :book_id, :book_type, :book_flags, :cat_id, :book_name, "
                        ":book_info, :author_name, :author_id, :file_name, :book_folder, :index_flags)");

    m_bookQuery.bindValue(":book_id", 0);
    m_bookQuery.bindValue(":book_type", LibraryBook::QuranBook);
    m_bookQuery.bindValue(":book_flags", 0);
    m_bookQuery.bindValue(":cat_id", 0);
    m_bookQuery.bindValue(":book_name", tr("القرآن الكريم"));
    m_bookQuery.bindValue(":book_info", QVariant(QVariant::String));
    m_bookQuery.bindValue(":author_name", QVariant(QVariant::String));
    m_bookQuery.bindValue(":author_id", 0);
    m_bookQuery.bindValue(":file_name", fileInfo.fileName()); // Add file name
    m_bookQuery.bindValue(":book_folder", QVariant(QVariant::String));
    m_bookQuery.bindValue(":index_flags", Enums::NotIndexed);

    if(!m_bookQuery.exec()) {
        LOG_SQL_ERROR(m_bookQuery);
    }
}

void LibraryCreator::readSimpleBook(ShamelaBookInfo *book, QSqlQuery &query, NewBookWriter &writer, bool hno)
{
    int lastID=0;
    bool mapPages = m_shamelaManager->getBookMateen(book->id) || m_shamelaManager->getBookShareeh(book->id);

    if(hno) {
        query.prepare(QString("SELECT id, nass, page, part, hno FROM %1 ORDER BY id").arg(book->mainTable));
        if(query.exec()) {
            while(query.next()) {
                lastID = writer.addPage(query.value(1).toString(),
                                        query.value(0).toInt(),
                                        query.value(2).toInt(),
                                        query.value(3).toInt());
                writer.addHaddithNumber(lastID, query.value(4).toInt());
                if(mapPages)
                    m_mapper->addPageMap(book->id, query.value(0).toInt(), lastID);
            }
        } else {
            LOG_SQL_ERROR(query);
        }
    } else {
        query.prepare(QString("SELECT id, nass, page, part FROM %1 ORDER BY id").arg(book->mainTable));
        if(query.exec()) {
            while(query.next()) {
                lastID = writer.addPage(query.value(1).toString(),
                                        query.value(0).toInt(),
                                        query.value(2).toInt(),
                                        query.value(3).toInt());
                if(mapPages)
                    m_mapper->addPageMap(book->id, query.value(0).toInt(), lastID);
            }
        } else {
            LOG_SQL_ERROR(query);
        }

    }
}

void LibraryCreator::readTafessirBook(ShamelaBookInfo *book, QSqlQuery &query, NewBookWriter &writer, bool hno)
{
    int lastID=0;
    if(hno) {
        query.prepare(QString("SELECT id, nass, page, part, aya, sora, hno FROM %1 ORDER BY id").arg(book->mainTable));
        if(query.exec()) {
            while(query.next()) {
                lastID = writer.addPage(query.value(1).toString(),
                                        query.value(0).toInt(),
                                        query.value(2).toInt(),
                                        query.value(3).toInt(),
                                        query.value(4).toInt(),
                                        query.value(5).toInt());
                writer.addHaddithNumber(lastID, query.value(6).toInt());
            }
        } else {
            LOG_SQL_ERROR(query);
        }
    } else {
        query.prepare(QString("SELECT id, nass, page, part, aya, sora FROM %1 ORDER BY id").arg(book->mainTable));
        if(query.exec()) {
            while(query.next()) {
                writer.addPage(query.value(1).toString(),
                               query.value(0).toInt(),
                               query.value(2).toInt(),
                               query.value(3).toInt(),
                               query.value(4).toInt(),
                               query.value(5).toInt());
            }
        } else {
            LOG_SQL_ERROR(query);
        }

    }
}

bool LibraryCreator::getShorooh(ShamelaBookInfo *mateen)
{
    int s_mateenID = mateen->id;
    int s_shareehID = m_shamelaManager->getBookShareeh(mateen->id);

    if(s_shareehID && s_mateenID) {
        getShorooh(s_mateenID, s_shareehID);
        return true;
    }

    return false;
}

bool LibraryCreator::getMateen(ShamelaBookInfo *shreeh)
{
    int s_shareehID = shreeh->id;
    int s_mateenID = m_shamelaManager->getBookMateen(shreeh->id);

    if(s_mateenID && s_shareehID) {
        getShorooh(s_mateenID, s_shareehID);
        return true;
    }

    return false;
}

void LibraryCreator::getShorooh(int mateenID, int shareehID)
{
    QList<ShamelaShareehInfo *> idsList = m_shamelaManager->getShareehInfo(mateenID, shareehID);
    m_shorooh.append(idsList);
}

QList<ShamelaShareehInfo *> LibraryCreator::getShorooh()
{
    return m_shorooh;
}

void LibraryCreator::addShareh(int mateenID, int mateenPage, int shareehID, int shareehPage)
{
    m_bookQuery.prepare("INSERT INTO ShareehMeta (mateen_book, mateen_id, shareeh_book, shareeh_id) "
                        "VALUES (?, ?, ?, ?)");
    m_bookQuery.bindValue(0, m_mapper->mapFromShamelaBook(mateenID));
    m_bookQuery.bindValue(1, m_mapper->mapFromShamelaPage(mateenID, mateenPage));
    m_bookQuery.bindValue(2, m_mapper->mapFromShamelaBook(shareehID));
    m_bookQuery.bindValue(3, m_mapper->mapFromShamelaPage(shareehID, shareehPage));

    if(!m_bookQuery.exec()) {
        LOG_SQL_ERROR(m_bookQuery);
    }
}

