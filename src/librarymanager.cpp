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

#include <qdebug.h>
#include <qsqlquery.h>
#include <qfile.h>
#include <qdir.h>
#include <qsqlerror.h>
#include <qdatetime.h>

LibraryManager::LibraryManager(LibraryInfo *info, QObject *parent) :
    QObject(parent),
    m_libraryInfo(info),
    m_bookmanager(0),
    m_taffesirManager(0),
    m_bookListManager(0),
    m_authorsManager(0)
{
    m_connName = "BooksIndexDB";
}

LibraryManager::~LibraryManager()
{
    if(m_bookmanager)
        delete m_bookmanager;

    if(m_bookListManager)
        delete m_bookListManager;

    if(m_taffesirManager)
        delete m_taffesirManager;
}

void LibraryManager::open()
{
    QString booksIndexPath = m_libraryInfo->booksIndexPath();

    if(!QFile::exists(booksIndexPath))
       qWarning("Can't find index database: %s", qPrintable(booksIndexPath));

    m_indexDB = QSqlDatabase::addDatabase("QSQLITE", m_connName);
    m_indexDB.setDatabaseName(booksIndexPath);

    if (!m_indexDB.open())
        throw BookException(tr("لم يمكن فتح قاعدة البيانات الأساسية"), m_indexDB.lastError().text());

    openManagers();
}

void LibraryManager::openManagers()
{
    m_authorsManager = new AuthorsManager(this); // should be the first
    m_bookmanager = new LibraryBookManager(this);
    m_bookListManager = new BookListManager(this);
    m_taffesirManager = new TaffesirListManager(this);
}

int LibraryManager::addBook(ImportModelNode *node)
{
    LibraryBook *book = node->toLibraryBook();
    QString newBookName = Utils::genBookName(m_libraryInfo->booksDir());
    QString newPath = m_libraryInfo->booksDir() + "/" + newBookName;

    if(QFile::copy(node->bookPath, newPath)){
        if(!QFile::remove(node->bookPath))
            qWarning() << "Can't remove:" << node->bookPath;

        book->fileName = newBookName;

        addBook(book, node->catID);

        return book->bookID;
    } else {
        qWarning() << "Can't copy" << node->bookPath << "to" << newPath;
        return -1;
    }
}

void LibraryManager::addBook(LibraryBook *book, int catID)
{
    m_bookmanager->addBook(book);
    m_bookListManager->addBook(book, catID);
}

void LibraryManager::setBookIndexStat(int bookID, Enums::indexFlags indexFlag)
{
    QSqlQuery bookQuery(m_indexDB);
    bookQuery.prepare("UPDATE booksList "
                      "SET indexFlags = ? "
                      "WHERE id = ?");

    bookQuery.bindValue(0, indexFlag);
    bookQuery.bindValue(1, bookID);

    if(!bookQuery.exec()) {
        LOG_SQL_ERROR(bookQuery);
    }
}

QList<int> LibraryManager::getNonIndexedBooks()
{
    QList<int> list;
    QSqlQuery bookQuery(m_indexDB);
    bookQuery.prepare("SELECT id FROM booksList "
                      "WHERE indexFlags = ?");

    bookQuery.bindValue(0, Enums::NotIndexed);

    if(bookQuery.exec()) {
        while(bookQuery.next()) {
            list << bookQuery.value(0).toInt();
        }
    } else {
        LOG_SQL_ERROR(bookQuery);
    }

    return list;
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
