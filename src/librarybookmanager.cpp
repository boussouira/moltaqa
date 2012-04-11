#include "librarybookmanager.h"
#include "mainwindow.h"
#include "librarymanager.h"
#include "libraryinfo.h"
#include "modelenums.h"
#include "utils.h"
#include "xmlutils.h"
#include "authorsmanager.h"

#include <qdir.h>
#include <qstandarditemmodel.h>

LibraryBookManager::LibraryBookManager(QObject *parent) :
    DatabaseManager(parent),
    m_quranBook(0)
{
    QDir dataDir(MW->libraryInfo()->dataDir());
    setDatabasePath(dataDir.filePath("books.db"));

    m_authorsManager = LibraryManager::instance()->authorsManager();
    Q_CHECK_PTR(m_authorsManager);

    openDatabase();
}

LibraryBookManager::~LibraryBookManager()
{
    clear();
}

void LibraryBookManager::loadModels()
{
}

void LibraryBookManager::clear()
{
    m_books.clear();
}

void LibraryBookManager::loadLibraryBooks()
{
    QSqlQuery query(m_db);
    query.prepare("SELECT id, title, type, authorID, author, info, bookFlags, indexFlags, filename "
                  "FROM books ORDER BY id");

    ML_QUERY_EXEC(query);

    while(query.next()) {
        LibraryBookPtr book(new LibraryBook());
        book->bookID = query.value(0).toInt();
        book->bookDisplayName = query.value(1).toString();
        book->bookType = static_cast<LibraryBook::Type>(query.value(2).toInt());
        book->bookInfo = query.value(5).toString();

        book->bookFlags = query.value(6).toInt();
        book->indexFlags = static_cast<LibraryBook::IndexFlags>(query.value(7).toInt());

        book->fileName = query.value(8).toString();
        book->bookPath = MW->libraryInfo()->bookPath(book->fileName);

        if(!book->isQuran()) {
            book->authorID = query.value(3).toInt();

            if(book->authorID) {
                book->authorName = m_authorsManager->getAuthorName(book->authorID);
            } else {
                book->authorName = query.value(4).toString();
            }
        }

        m_books.insert(book->bookID, book);
    }
}

QStandardItemModel *LibraryBookManager::getModel()
{
    QStandardItemModel *model = new QStandardItemModel();

    QSqlQuery query(m_db);
    query.prepare("SELECT id, title FROM books ORDER BY id");

    ML_QUERY_EXEC(query);

    while(query.next()) {
        QStandardItem *item = new QStandardItem();
        item->setText(query.value(1).toString());
        item->setData(query.value(0).toInt(), ItemRole::idRole);
        item->setIcon(QIcon(":/images/book.png"));

        model->appendRow(item);
    }

    model->setHorizontalHeaderLabels(QStringList() << tr("الكتب"));

    return model;
}

LibraryBookPtr LibraryBookManager::getLibraryBook(int bookID)
{
    LibraryBookPtr book = m_books.value(bookID);
    if(book)
        return book;

    QSqlQuery query(m_db);
    query.prepare("SELECT id, title, type, authorID, author, info, bookFlags, indexFlags, filename "
                  "FROM books WHERE id = ?");
    query.bindValue(0, bookID);

    ML_QUERY_EXEC(query);

    if(query.next()) {
        LibraryBookPtr book(new LibraryBook());
        book->bookID = query.value(0).toInt();
        book->bookDisplayName = query.value(1).toString();
        book->bookType = static_cast<LibraryBook::Type>(query.value(2).toInt());
        book->bookInfo = query.value(5).toString();

        book->bookFlags = query.value(6).toInt();
        book->indexFlags = static_cast<LibraryBook::IndexFlags>(query.value(7).toInt());

        book->fileName = query.value(8).toString();
        book->bookPath = MW->libraryInfo()->bookPath(book->fileName);

        if(!book->isQuran()) {
            book->authorID = query.value(3).toInt();

            if(book->authorID) {
                book->authorName = m_authorsManager->getAuthorName(book->authorID);
            } else {
                book->authorName = query.value(4).toString();
            }
        }

        m_books.insert(book->bookID, book);

        return book;
    }

    return LibraryBookPtr();
}

LibraryBookPtr LibraryBookManager::getQuranBook()
{
    if(m_quranBook)
        return m_quranBook;

    QSqlQuery query(m_db);
    query.prepare("SELECT id FROM books WHERE type = ?");
    query.bindValue(0, LibraryBook::QuranBook);

    ML_QUERY_EXEC(query);

    if(query.next()) {
        m_quranBook = getLibraryBook(query.value(0).toInt());
        if(m_quranBook)
            return m_quranBook;
    }

    return LibraryBookPtr();
}

int LibraryBookManager::addBook(LibraryBookPtr book)
{
    QMutexLocker locker(&m_mutex);

    if(!book->bookID)
        book->bookID = getNewBookID();

    QSqlQuery query(m_db);

    Utils::QueryBuilder q;
    q.setTableName("books");
    q.setQueryType(Utils::QueryBuilder::Insert);

    q.addColumn("id", book->bookID);
    q.addColumn("title", book->bookDisplayName);
    q.addColumn("type", book->bookType);
    q.addColumn("authorID", book->authorID);
    q.addColumn("author", (!book->authorID) ? book->authorName : QVariant(QVariant::String));
    q.addColumn("info", book->bookInfo);
    q.addColumn("bookFlags", book->bookFlags);
    q.addColumn("indexFlags", book->indexFlags);
    q.addColumn("filename", book->fileName);

    ML_ASSERT_RET(q.exec(query), 0);

    m_books.insert(book->bookID, book);
    return book->bookID;
}

bool LibraryBookManager::updateBook(LibraryBookPtr book)
{
    QSqlQuery query(m_db);

    Utils::QueryBuilder q;
    q.setTableName("books");
    q.setQueryType(Utils::QueryBuilder::Update);

    q.addColumn("title", book->bookDisplayName);
    q.addColumn("type", book->bookType);
    q.addColumn("authorID", book->authorID);
    q.addColumn("author", (!book->authorID) ? book->authorName : QVariant(QVariant::String));
    q.addColumn("info", book->bookInfo);
    q.addColumn("bookFlags", book->bookFlags);
    q.addColumn("indexFlags", book->indexFlags);
    q.addColumn("filename", book->fileName);

    q.addWhere("id", book->bookID);

    ML_ASSERT_RET(q.exec(query), false);

    m_books.insert(book->bookID, book);
    return true;
}

bool LibraryBookManager::removeBook(int bookID)
{
    m_query.prepare("DELETE FROM books WHERE id = ?");
    m_query.bindValue(0, bookID);
    if(m_query.exec()) {
        m_books.remove(bookID);
        return true;
    } else {
        LOG_SQL_ERROR(m_query);
        return false;
    }
}

QList<int> LibraryBookManager::getBooksWithIndexStat(LibraryBook::IndexFlags indexFlag)
{
    QList<int> list;
    QSqlQuery query(m_db);
    query.prepare("SELECT id FROM books WHERE indexFlags = ?");

    query.bindValue(0, indexFlag);

    if(query.exec()) {
        while(query.next()) {
            list << query.value(0).toInt();
        }
    } else {
        LOG_SQL_ERROR(query);
    }

    return list;
}

void LibraryBookManager::setBookIndexStat(int bookID, LibraryBook::IndexFlags indexFlag)
{
    QSqlQuery query(m_db);

    Utils::QueryBuilder q;
    q.setTableName("books");
    q.setQueryType(Utils::QueryBuilder::Update);

    q.addColumn("indexFlags", indexFlag);
    q.addWhere("id", bookID);

    ML_ASSERT(q.exec(query));

    LibraryBookPtr book = m_books.value(bookID);
    ML_ASSERT2(book, "LibraryBookManager::setBookIndexStat No book with id" << bookID << "where found");

    book->indexFlags = indexFlag;
}

QList<LibraryBookPtr> LibraryBookManager::getAuthorBooks(int authorID)
{
    QList<LibraryBookPtr> list;

    QSqlQuery query(m_db);
    query.prepare("SELECT id FROM books WHERE authorID = ?");
    query.bindValue(0, authorID);

    ML_QUERY_EXEC(query);

    while(query.next()) {
        LibraryBookPtr book = getLibraryBook(query.value(0).toInt());
        if(book)
            list.append(book);
    }

    return list;
}

int LibraryBookManager::getNewBookID()
{
    int bookID =  0;

    do {
        bookID = Utils::randInt(111111, 999999);
    } while(m_books.contains(bookID));

    return bookID;
}

