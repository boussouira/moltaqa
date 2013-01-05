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
#include "bookutils.h"

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
}

void LibraryCreator::openDB()
{
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
        ml_return_on_fail(!lid); // This author is already in the map

        // We look for this author in the index database
        AuthorInfo::Ptr foundAuthor = m_authorsManager->findAuthor(auth->name, false);

        // If found the author in our database so add it to the map and return
        if(foundAuthor) {
            m_mapper->addAuthorMap(auth->id, foundAuthor->id);
            return;
        }
    }

    // Add this author from shamela
    int insertAuthor = m_authorsManager->addAuthor(AuthorInfo::Ptr(auth->toAuthorInfo()));
    m_mapper->addAuthorMap(auth->id, insertAuthor);
}

void LibraryCreator::addBook(ShamelaBookInfo *book)
{
    DatabaseRemover remover;
    QString connName(QString("mdb_%1_%2").arg(m_threadID).arg(book->archive));

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
        ml_throw_on_db_open_fail(bookDB);
    }

    NewBookWriter bookWrite;
    bookWrite.createNewBook();
    bookWrite.startReading();

    QString tableName = book->mainTable;
    QString queryFields = Utils::Book::shamelaQueryFields(bookDB, tableName);

    QSqlQuery query(bookDB);
    query.prepare(QString("SELECT %1 FROM %2 ORDER BY id")
                  .arg(queryFields)
                  .arg(tableName));

    ml_throw_on_query_exec_fail(query);

    int IdCol = query.record().indexOf("id");
    int nassCol = query.record().indexOf("nass");
    int pageCol = query.record().indexOf("page");
    int partCol = query.record().indexOf("part");
    int ayaCol = query.record().indexOf("aya");
    int soraCol = query.record().indexOf("sora");
    int hnoCol = query.record().indexOf("hno");

    BookPage page;
    ShoortsList shoorts = m_shamelaManager->getBookShoorts(book->id);
    while(query.next()) {
        page.pageID = query.value(IdCol).toInt();
        page.page = query.value(pageCol).toInt();
        page.part = query.value(partCol).toInt();

        if(soraCol != -1 && ayaCol != -1) {
            page.aya = query.value(ayaCol).toInt();
            page.sora = query.value(soraCol).toInt();
        }

        if(hnoCol != -1) {
            page.haddit = query.value(hnoCol).toInt();
        }

        if(shoorts.size())
            RemoveShamelaShoorts(page.text, shoorts);

        bookWrite.addPage(&page);
        page.clear();
    }

    query.prepare(QString("SELECT id, tit, lvl, sub FROM %1 ORDER BY id, sub").arg(book->tocTable));
    ml_throw_on_query_exec_fail(query);

    while(query.next()) {
        bookWrite.addTitle(query.value(1).toString(),
                           query.value(0).toInt(),
                           query.value(2).toInt());
    }

    query.prepare(QString("SELECT id, nass FROM %1").arg(book->mainTable));
    ml_throw_on_query_exec_fail(query);

    IdCol = query.record().indexOf("id");
    nassCol = query.record().indexOf("nass");
    while(query.next()) {
        bookWrite.addPageText(query.value(IdCol).toInt(),
                              query.value(nassCol).toString());
    }

    bookWrite.endReading();

    if(!book->archive)
        remover.removeDatabase(connName);

    importBook(book, bookWrite.bookPath());

    if(book->tafessirName.size()) {
        addTafessir(book);
    }
}

void LibraryCreator::addQuran()
{
    QString connName(QString("shamela_quran_%1").arg(m_threadID));
    QString path = Utils::Rand::newBook(m_library->booksDir());
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

        ml_throw_on_db_open_fail(bookDB);

        QSqlQuery query(bookDB);
        quranWrite.startReading();

        query.prepare("SELECT nass, sora, aya, Page FROM Qr ORDER BY id");
        ml_throw_on_query_exec_fail(query);

        while(query.next()) {
            quranWrite.addPage(query.value(0).toString(),
                               query.value(1).toInt(),
                               query.value(2).toInt(),
                               query.value(3).toInt());
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
}

void LibraryCreator::done()
{
}

void LibraryCreator::importBook(ShamelaBookInfo *shamelBook, QString path)
{
    QFileInfo fileInfo(path);

    ShamelaAuthorInfo *auth = m_shamelaManager->getAuthorInfo(shamelBook->authorID);
    if(auth) {
        addAuthor(auth, true);
        delete auth;
    }

    LibraryBook::Ptr book(new LibraryBook());
    book->type = shamelBook->tafessirName.isEmpty() ? LibraryBook::NormalBook : LibraryBook::TafessirBook;
    book->title = shamelBook->name;
    book->comment = shamelBook->betaka.append(tr("\n[مأخود من الشاملة]"));
    book->info = shamelBook->info;
    book->authorID = m_mapper->mapFromShamelaAuthor(shamelBook->authorID);
    book->authorName = shamelBook->authName;
    book->fileName = fileInfo.fileName();

    m_libraryManager->addBook(book, m_mapper->mapFromShamelaCat(shamelBook->cat));

    m_mapper->addBookMap(shamelBook->id, book->id);
}

void LibraryCreator::importQuran(QString path)
{
    QFileInfo fileInfo(path);

    LibraryBook::Ptr book(new LibraryBook());
    book->type = LibraryBook::QuranBook;
    book->title = tr("القرآن الكريم");
    book->info = tr("القرآن الكريم");
    book->fileName = fileInfo.fileName();

    m_libraryManager->addBook(book, 0);
}
