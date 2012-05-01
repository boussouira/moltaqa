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

#include <qdebug.h>
#include <qsqlquery.h>
#include <qfile.h>
#include <qdir.h>
#include <qsqlerror.h>
#include <qdatetime.h>

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
    m_instance = this;

    QDir dataDir(info->dataDir());
    setDatabasePath(dataDir.filePath("library.db"));

    //openDatabase();
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

    m_managers.clear();

    m_managers << m_authorsManager
               << m_favourites
               << m_bookmanager
               << m_bookListManager
               << m_taffesirManager
               << m_rowatManager
               << m_searchManager;

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
    LibraryBookPtr book = node->toLibraryBook();
    QString newBookName = Utils::Rand::fileName(m_libraryInfo->booksDir());
    QString newPath = m_libraryInfo->booksDir() + "/" + newBookName;

    if(QFile::copy(node->bookPath, newPath)){
        if(!QFile::remove(node->bookPath))
            qWarning() << "LibraryManager::addBook Can't remove:" << node->bookPath;

        book->fileName = newBookName;

        addBook(book, node->catID);

        return book->id;
    } else {
        qWarning() << "LibraryManager::addBook Can't copy" << node->bookPath << "to" << newPath;
        return -1;
    }
}

void LibraryManager::addBook(LibraryBookPtr book, int catID)
{
    m_bookmanager->addBook(book);
    m_bookListManager->addBook(book, catID);
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
