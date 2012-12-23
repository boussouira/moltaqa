#include "librarymanager.h"
#include "libraryinfo.h"
#include "librarybook.h"
#include "bookexception.h"
#include "importmodel.h"
#include "utils.h"
#include "searchresult.h"
#include "xmlutils.h"
#include "librarybookmanager.h"
#include "taffesirlistmanager.h"
#include "booklistmanager.h"
#include "authorsmanager.h"
#include "bookeditor.h"
#include "tarajemrowatmanager.h"
#include "favouritesmanager.h"
#include "searchmanager.h"
#include "indextracker.h"
#include "statisticsmanager.h"

#include <qdebug.h>
#include <qsqlquery.h>
#include <qfile.h>
#include <qdir.h>
#include <qsqlerror.h>
#include <qdatetime.h>
#include <qaction.h>

static LibraryManager *m_instance = 0;

LibraryManager::LibraryManager(LibraryInfo *info, QObject *parent) :
    DatabaseManager(parent),
    m_libraryInfo(info),
    m_bookmanager(0),
    m_taffesirManager(0),
    m_bookListManager(0),
    m_authorsManager(0),
    m_rowatManager(0),
    m_favourites(0)
{
    ml_set_instance(m_instance, this);

    QDir dataDir(info->dataDir());
    setDatabasePath(dataDir.filePath("library.db"));

    openDatabase();
    openManagers();
}

LibraryManager::~LibraryManager()
{
    foreach (ListManager *manager, m_managers) {
        ml_delete_check(manager);
    }

    m_instance = 0;
}

LibraryManager *LibraryManager::instance()
{
    return m_instance;
}

LibraryInfo *LibraryManager::libraryInfo()
{
    return m_libraryInfo;
}

void LibraryManager::loadModels()
{
}

void LibraryManager::clear()
{
}

void LibraryManager::openManagers()
{
    m_authorsManager = new AuthorsManager(this); // should be the first
    m_favourites = new FavouritesManager(this);
    m_bookmanager = new LibraryBookManager(this);
    m_bookListManager = new BookListManager(this);
    m_taffesirManager = new TaffesirListManager(this);
    m_rowatManager = new TarajemRowatManager(this);
    m_searchManager = new SearchManager(this);
    m_statisticsManager = new StatisticsManager(this);

    m_managers.clear();

    m_managers << m_authorsManager
               << m_favourites
               << m_bookmanager
               << m_bookListManager
               << m_taffesirManager
               << m_rowatManager
               << m_searchManager
               << m_statisticsManager;

    foreach (ListManager *manager, m_managers)
        manager->loadModels();
}

void LibraryManager::reloadManagers()
{
    foreach (ListManager *manager, m_managers)
        manager->reloadModels();
}

int LibraryManager::addBook(ImportModelNode *node)
{
    LibraryBook::Ptr book = node->toLibraryBook();
    QString bookPath = (book->fileName.startsWith("book_") ? m_libraryInfo->bookPath(book->fileName) :  QString());

    QString newPath;
    if(bookPath.isEmpty() || book->fileName.isEmpty() || QFile::exists(bookPath)) {
        newPath = Utils::Rand::newBook(m_libraryInfo->booksDir());
        book->fileName = QFileInfo(newPath).fileName();
    } else {
        newPath = bookPath;
    }

    if(QFile::copy(node->path, newPath)){
        if(!QFile::remove(node->path))
            qWarning() << "LibraryManager::addBook Can't remove:" << node->path;

        addBook(book, node->catID);

        return book->id;
    } else {
        qWarning() << "LibraryManager::addBook Can't copy" << node->path << "to" << newPath;
        return -1;
    }
}

void LibraryManager::addBook(LibraryBook::Ptr book, int catID)
{
    int bookID = m_bookmanager->addBook(book);
    if(!bookID) {
        throw BookException(QString("LibraryManager::addBook Error when adding book '%1' to the database")
                            .arg(book->title));
    }

    m_bookListManager->addBook(book, catID);
}

void LibraryManager::removeBook(int bookID)
{
    ml_return_on_fail2(m_bookmanager->removeBook(bookID),
                       "LibraryManager::removeBook can't remove book" << bookID);

    ml_warn_on_fail(m_bookListManager->removeBook(bookID),
                    "BookListManager::removeBook no book with id" << bookID);

    m_favourites->removeBook(bookID);

    IndexTracker::instance()->addTask(bookID, IndexTask::Delete, true);
}

QHash<int, QAction *> LibraryManager::textRefersActions()
{
    QSqlQuery query(m_db);
    query.prepare("SELECT id, name, referText FROM refers");
    ml_query_exec(query);

    QHash<int, QAction *> hash;
    while(query.next()) {
        QAction *act = new QAction(this);
        act->setText(query.value(1).toString());
        act->setData(query.value(2).toString());

        hash[query.value(0).toInt()] = act;
    }

    return hash;
}

void LibraryManager::addTextRefers(const QString &name, const QString &referText)
{
    QueryBuilder q;
    q.setTableName("refers", QueryBuilder::Insert);
    q.set("name", name);
    q.set("referText", referText);
    q.exec(m_db);
}

void LibraryManager::editRefers(int rid, const QString &name, const QString &referText)
{
    QueryBuilder q;
    q.setTableName("refers", QueryBuilder::Update);
    q.set("name", name);
    q.set("referText", referText);
    q.where("id", rid);
    q.exec(m_db);
}

void LibraryManager::deleteRefer(int rid)
{
    QueryBuilder q;
    q.setTableName("refers", QueryBuilder::Delete);
    q.where("id", rid);
    q.exec(m_db);
}

QHash<QString, QVariant> LibraryManager::libraryStatistic()
{
    QHash<QString, QVariant> s;
    s["books_count"] = m_bookmanager->booksCount();
    s["authors_count"] = m_authorsManager->authorsCount();
    s["rowat_count"] = m_rowatManager->rowatCount();
    s["categories_count"] = m_bookListManager->categoriesCount();

    return s;
}

TaffesirListManager *LibraryManager::taffesirListManager()
{
    return m_taffesirManager;
}

BookListManager *LibraryManager::bookListManager()
{
    return m_bookListManager;
}

LibraryBookManager *LibraryManager::bookManager()
{
    return m_bookmanager;
}

AuthorsManager *LibraryManager::authorsManager()
{
    return m_authorsManager;
}

TarajemRowatManager *LibraryManager::rowatManager()
{
    return m_rowatManager;
}

FavouritesManager *LibraryManager::favouritesManager()
{
    return m_favourites;
}

SearchManager *LibraryManager::searchManager()
{
    return m_searchManager;
}

void LibraryManager::addHelpBook()
{
    if(!m_bookmanager->getLibraryBook(HELP_BOOK_ID)) {
        qDebug("LibraryManager::addHelpBook add help book to the current library...");

        LibraryBook::Ptr book(new LibraryBook());
        book->id = HELP_BOOK_ID;
        book->type = LibraryBook::NormalBook;
        book->title = tr("شرح البرنامج");
        book->info = tr("شرح استخدام برنامج مكتبة الملتقى");
        book->authorName = tr("مكتبة الملتقى");
        book->fileName = "${DATA_DIR}/help/help.mlb";

        int bookID = m_bookmanager->addBook(book);
        if(bookID)
            IndexTracker::instance()->addTask(bookID, IndexTask::Add, false);
        else
            qWarning() << "LibraryManager::addHelpBook Can't add help book";
    }
}
