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

#include <qdebug.h>
#include <qsqlquery.h>
#include <qfile.h>
#include <qdir.h>
#include <qsqlerror.h>
#include <qdatetime.h>

LibraryManager::LibraryManager(LibraryInfo *info, QObject *parent) : QObject(parent)
{
    m_taffesirManager = 0;

    m_libraryInfo = info;
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

LibraryInfo *LibraryManager::connectionInfo()
{
    return m_libraryInfo;
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
    m_bookmanager = new LibraryBookManager(this);
    m_bookListManager = new BookListManager(this);
    m_taffesirManager = new TaffesirListManager(this);
}

QPair<int, QString> LibraryManager::findAuthor(const QString &name)
{
    QPair<int, QString> foundAuth;
    int count = 0;

    if(name.isEmpty()) {
        foundAuth.first = 0;
        return foundAuth;
    }

    QSqlQuery bookQuery(m_indexDB);
    if(bookQuery.exec(QString("SELECT id, name FROM authorsList WHERE %1").arg(Sql::buildLikeQuery(name, "name")))) {
        while(bookQuery.next()) {
            foundAuth.first = bookQuery.value(0).toInt();
            foundAuth.second = bookQuery.value(1).toString();

            if(name == bookQuery.value(1).toString()) {
                count = 1;
                break;
            } else {
                count++;
            }
        }
    } else {
        LOG_SQL_ERROR(bookQuery)
    }

    if(count != 1) {
        foundAuth.first = 0;
        foundAuth.second.clear();
    }

    return foundAuth;
}

bool LibraryManager::hasShareeh(int bookID)
{
    QSqlQuery bookQuery(m_indexDB);
    int count = 0;

    bookQuery.prepare("SELECT COUNT(*) FROM ShareehMeta WHERE "
                      "mateen_book = ?");
    bookQuery.bindValue(0, bookID);
    if(bookQuery.exec()) {
        if(bookQuery.next()) {
            count = bookQuery.value(0).toInt();
        }
    } else {
        LOG_SQL_ERROR(bookQuery);
    }

    return count;
}

int LibraryManager::addBook(ImportModelNode *node)
{
    LibraryBook *book = node->toLibraryBook();
    QString newBookName = Utils::genBookName(m_libraryInfo->booksDir());
    QString newPath = m_libraryInfo->booksDir() + "/" + newBookName;

    if(QFile::copy(node->bookPath, newPath)){
        if(!QFile::remove(node->bookPath))
            qWarning() << "Can't remove:" << node->bookPath;

        int bookID = m_bookmanager->getNewBookID();
        book->bookID = bookID;
        book->fileName = newBookName;

        addBook(book, node->catID);

        delete book;
        return bookID;
    } else {
        qWarning() << "Can't copy" << node->bookPath << "to" << newPath;
        return -1;
    }
}

void LibraryManager::addBook(LibraryBook *book, int catID)
{
    QMutexLocker locker(&m_mutex);

    m_bookmanager->addBook(book);
    m_bookListManager->addBook(book, catID);
}

void LibraryManager::getShoroohPages(LibraryBook *info, BookPage *page)
{
    qDeleteAll(info->shorooh);
    info->shorooh.clear();

    QSqlQuery bookQuery(m_indexDB);

    bookQuery.prepare("SELECT ShareehMeta.shareeh_book, ShareehMeta.shareeh_id, booksList.bookDisplayName "
                      "FROM ShareehMeta "
                      "LEFT JOIN booksList "
                      "ON booksList.id =  ShareehMeta.shareeh_book "
                      "WHERE ShareehMeta.mateen_book = ? AND ShareehMeta.mateen_id = ?");
    bookQuery.bindValue(0, info->bookID);
    bookQuery.bindValue(1, page->pageID);
    if(!bookQuery.exec()) {
        LOG_SQL_ERROR(bookQuery);
    }

    while(bookQuery.next()) {
        info->shorooh.append(new BookShorooh(bookQuery.value(0).toInt(),
                                             bookQuery.value(1).toInt(),
                                             bookQuery.value(2).toString()));
    }
}

QStandardItemModel *LibraryManager::getAuthorsListModel()
{
    QStandardItemModel *model = new QStandardItemModel();
    QStandardItem *item;
    QSqlQuery bookQuery(m_indexDB);
    bookQuery.prepare("SELECT id, name, full_name FROM authorsList");
    if(bookQuery.exec()) {
        while(bookQuery.next()) {
            item = new QStandardItem(bookQuery.value(1).toString());
            item->setData(bookQuery.value(0).toInt(), Qt::UserRole);
            item->setData(bookQuery.value(2).toString(), Qt::ToolTipRole);

            model->appendRow(item);
        }
    } else {
        LOG_SQL_ERROR(bookQuery);
    }

    return model;
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
