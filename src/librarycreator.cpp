#include "librarycreator.h"
#include "utils.h"
#include "newbookwriter.h"
#include "librarybook.h"
#include "shamelaimportdialog.h"
#include "newquranwriter.h"
#include "mainwindow.h"
#include "libraryenums.h"
#include "taffesirlistmanager.h"
#include "booklistmanager.h"
#include "librarybookmanager.h"
#include "authorsmanager.h"

#ifdef USE_MDBTOOLS
#include "mdbconverter.h"
#endif

#include <qdebug.h>
#include <qvariant.h>
#include <qfileinfo.h>
#include <qsqlrecord.h>

void RemoveShamelaShoorts(QString &pageText, ShoortsList &shoorts) {
    for(int i=0; i < shoorts.count(); i++) {
        if(shoorts.at(i).second[0] == '[')
            pageText.replace(shoorts.at(i).first, '\n' + shoorts.at(i).second + '\n');
        else
            pageText.replace(shoorts.at(i).first, '\n' + shoorts.at(i).second);
    }
}

LibraryCreator::LibraryCreator()
{
    ShamelaImportDialog *importDialog = ShamelaImportDialog::instance();

    m_shamelaManager = importDialog->shamelaManager();
    m_shamelaInfo = importDialog->shamelaInfo();
    m_library = importDialog->libraryInfo();
    m_mapper = m_shamelaManager->mapper();

    m_libraryManager = LibraryManager::instance();
    m_authorsManager = m_libraryManager->authorsManager();
    m_taffesirManager = m_libraryManager->taffesirListManager();
    m_bookListManager = m_libraryManager->bookListManager();

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
    Utils::createIndexDB(m_library->path());
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
    m_taffesirManager->addTafessir(m_mapper->mapFromShamelaBook(tafessir->id),
                                   tafessir->tafessirName);

    qDebug() << "Add tafessir:" << tafessir->tafessirName;
}

void LibraryCreator::addCat(ShamelaCategorieInfo *cat)
{
    int lastId = m_bookListManager->addCategorie(cat->name,
                                                 m_levels.value(cat->level-1, 0));

    m_mapper->addCatMap(cat->id, lastId);

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

    if(checkExist) {
        int lid = m_mapper->mapFromShamelaAuthor(auth->id);
        ML_ASSERT(!lid); // This author is already in the map

        // We look for this author in the index database
        AuthorInfoPtr foundAuthor = m_authorsManager->findAuthor(auth->name);

        // If found the author in our database so add it to the map and return
        if(foundAuthor) {
            m_mapper->addAuthorMap(auth->id, foundAuthor->id);
            return;
        }
    }

    // Add this author from shamela
    int insertAuthor = m_authorsManager->addAuthor(AuthorInfoPtr(auth->toAuthorInfo()));
    m_mapper->addAuthorMap(auth->id, insertAuthor);
}

void LibraryCreator::addBook(ShamelaBookInfo *book)
{
    DatabaseRemover remover;
    QString connName(QString("mdb_%1_%2").arg(m_threadID).arg(book->archive));
    QString path = Utils::Rand::fileName(m_library->booksDir(), true);

    NewBookWriter bookWrite;
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

    query.prepare(QString("SELECT id, tit, lvl, sub FROM %1 ORDER BY id, sub").arg(book->tocTable));
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

    if(!book->tafessirName.isEmpty()) {
        addTafessir(book);
    }
}

void LibraryCreator::addQuran()
{
    QString connName(QString("shamela_quran_%1").arg(m_threadID));
    QString path = Utils::Rand::fileName(m_library->booksDir(), true);
    QString tempDB;

    {
        newQuranWriter quranWrite;
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

void LibraryCreator::importBook(ShamelaBookInfo *shamelBook, QString path)
{
    QFileInfo fileInfo(path);

    ShamelaAuthorInfo *auth = m_shamelaManager->getAuthorInfo(shamelBook->authorID);
    if(auth) {
        addAuthor(auth, true);
        delete auth;
    }

    LibraryBookPtr book(new LibraryBook());
    book->bookType = shamelBook->tafessirName.isEmpty() ? LibraryBook::NormalBook : LibraryBook::TafessirBook;
    book->bookDisplayName = shamelBook->name;
    book->bookInfo = shamelBook->info;
    book->authorID = m_mapper->mapFromShamelaAuthor(shamelBook->authorID);
    book->authorName = shamelBook->authName;
    book->fileName = fileInfo.fileName();

    m_libraryManager->addBook(book, m_mapper->mapFromShamelaCat(shamelBook->cat));

    m_mapper->addBookMap(shamelBook->id, book->bookID);
}

void LibraryCreator::importQuran(QString path)
{
    QFileInfo fileInfo(path);

    LibraryBookPtr book(new LibraryBook());
    book->bookType = LibraryBook::QuranBook;
    book->bookDisplayName = tr("القرآن الكريم");
    book->bookInfo = tr("القرآن الكريم");
    book->fileName = fileInfo.fileName();

    m_libraryManager->addBook(book, 0);
}

void LibraryCreator::readSimpleBook(ShamelaBookInfo *book, QSqlQuery &query, NewBookWriter &writer, bool hno)
{
    ShoortsList shoorts = m_shamelaManager->getBookShoorts(book->id);

    if(hno) {
        query.prepare(QString("SELECT id, nass, page, part, hno FROM %1 ORDER BY id").arg(book->mainTable));
        if(query.exec()) {
            while(query.next()) {
                QString pageText = query.value(1).toString();
                RemoveShamelaShoorts(pageText, shoorts);

                writer.addPage(pageText,
                               query.value(0).toInt(),
                               query.value(2).toInt(),
                               query.value(3).toInt(),
                               query.value(4).toInt());
            }
        } else {
            LOG_SQL_ERROR(query);
        }
    } else {
        query.prepare(QString("SELECT id, nass, page, part FROM %1 ORDER BY id").arg(book->mainTable));
        if(query.exec()) {
            while(query.next()) {
                QString pageText= query.value(1).toString();
                RemoveShamelaShoorts(pageText, shoorts);

                writer.addPage(pageText,
                               query.value(0).toInt(),
                               query.value(2).toInt(),
                               query.value(3).toInt());
            }
        } else {
            LOG_SQL_ERROR(query);
        }

    }
}

void LibraryCreator::readTafessirBook(ShamelaBookInfo *book, QSqlQuery &query, NewBookWriter &writer, bool hno)
{
    ShoortsList shoorts = m_shamelaManager->getBookShoorts(book->id);

    if(hno) {
        query.prepare(QString("SELECT id, nass, page, part, aya, sora, hno FROM %1 ORDER BY id").arg(book->mainTable));
        if(query.exec()) {
            while(query.next()) {
                QString pageText= query.value(1).toString();
                RemoveShamelaShoorts(pageText, shoorts);

                writer.addPage(pageText,
                               query.value(0).toInt(),
                               query.value(2).toInt(),
                               query.value(3).toInt(),
                               query.value(6).toInt(),
                               query.value(4).toInt(),
                               query.value(5).toInt());
            }
        } else {
            LOG_SQL_ERROR(query);
        }
    } else {
        query.prepare(QString("SELECT id, nass, page, part, aya, sora FROM %1 ORDER BY id").arg(book->mainTable));
        if(query.exec()) {
            while(query.next()) {
                QString pageText= query.value(1).toString();
                RemoveShamelaShoorts(pageText, shoorts);

                writer.addPage(pageText,
                               query.value(0).toInt(),
                               query.value(2).toInt(),
                               query.value(3).toInt(),
                               -1,
                               query.value(4).toInt(),
                               query.value(5).toInt());
            }
        } else {
            LOG_SQL_ERROR(query);
        }

    }
}
