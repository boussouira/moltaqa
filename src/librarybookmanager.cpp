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
    m_libraryInfo = MW->libraryInfo();

    QDir dataDir(m_libraryInfo->dataDir());
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

QStandardItemModel *LibraryBookManager::getLastOpendModel()
{
    QStandardItemModel *model = new QStandardItemModel();

    QSqlQuery query(m_db);//    0              1                 2                 3
    query.prepare("SELECT last_open.book, last_open.page, last_open.open_date, books.title "
                  "FROM last_open "
                  "LEFT JOIN books "
                  "ON books.id = last_open.book "
                  "ORDER BY open_date DESC");

    ML_QUERY_EXEC(query);

    while(query.next()) {
        QStandardItem *item = new QStandardItem();
        item->setText(query.value(3).toString());
        item->setData(query.value(1).toInt(), ItemRole::idRole);
        item->setData(query.value(0).toInt(), ItemRole::bookIdRole);
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
    query.prepare("SELECT id, title, type, authorID, author, info, bookFlags, indexFlags, filename, "
                  "otherTitles, edition, publisher, mohaqeq, comment "
                  "FROM books WHERE id = ?");
    query.bindValue(0, bookID);

    ML_QUERY_EXEC(query);

    if(query.next()) {
        LibraryBookPtr book(new LibraryBook());
        book->id = query.value(0).toInt();
        book->title = query.value(1).toString();
        book->type = static_cast<LibraryBook::Type>(query.value(2).toInt());
        book->info = query.value(5).toString();
        book->comment = query.value(13).toString();

        book->otherTitles = query.value(9).toString();
        book->edition = query.value(10).toString();
        book->publisher = query.value(11).toString();
        book->mohaqeq = query.value(12).toString();

        book->bookFlags = query.value(6).toInt();
        book->indexFlags = static_cast<LibraryBook::IndexFlags>(query.value(7).toInt());

        book->fileName = query.value(8).toString();
        book->path = m_libraryInfo->bookPath(book->fileName);

        if(!book->isQuran()) {
            book->authorID = query.value(3).toInt();

            if(book->authorID) {
                book->authorName = m_authorsManager->getAuthorName(book->authorID);
            } else {
                book->authorName = query.value(4).toString();
            }
        }

        m_books.insert(book->id, book);

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

    if(!book->id)
        book->id = getNewBookID();

    QSqlQuery query(m_db);

    QueryBuilder q;
    q.setTableName("books");
    q.setQueryType(QueryBuilder::Insert);

    q.set("id", book->id);
    q.set("title", book->title);
    q.set("otherTitles", book->otherTitles);
    q.set("type", book->type);
    q.set("authorID", book->authorID);
    q.set("author", (!book->authorID) ? book->authorName : QVariant(QVariant::String));
    q.set("comment", book->comment);
    q.set("info", book->info);
    q.set("edition", book->edition);
    q.set("publisher", book->publisher);
    q.set("mohaqeq", book->mohaqeq);
    q.set("bookFlags", book->bookFlags);
    q.set("indexFlags", book->indexFlags);
    q.set("filename", book->fileName);

    ML_ASSERT_RET(q.exec(query), 0);

    m_books.insert(book->id, book);
    return book->id;
}

bool LibraryBookManager::updateBook(LibraryBookPtr book)
{
    QSqlQuery query(m_db);

    QueryBuilder q;
    q.setTableName("books");
    q.setQueryType(QueryBuilder::Update);

    q.set("title", book->title);
    q.set("otherTitles", book->otherTitles);
    q.set("type", book->type);
    q.set("authorID", book->authorID);
    q.set("author", (!book->authorID) ? book->authorName : QVariant(QVariant::String));
    q.set("comment", book->comment);
    q.set("info", book->info);
    q.set("edition", book->edition);
    q.set("publisher", book->publisher);
    q.set("mohaqeq", book->mohaqeq);
    q.set("bookFlags", book->bookFlags);
    q.set("indexFlags", book->indexFlags);
    q.set("filename", book->fileName);

    q.where("id", book->id);

    ML_ASSERT_RET(q.exec(query), false);

    m_books.insert(book->id, book);
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

    QueryBuilder q;
    q.setTableName("books");
    q.setQueryType(QueryBuilder::Update);

    q.set("indexFlags", indexFlag);
    q.where("id", bookID);

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

void LibraryBookManager::addBookHistory(int bookID, int pageID)
{
    QSqlQuery query(m_db);

    qint64 t = QDateTime::currentDateTime().toMSecsSinceEpoch();

    QueryBuilder q;
    q.setTableName("history", QueryBuilder::Insert);

    q.set("book", bookID);
    q.set("page", pageID);
    q.set("open_date", t);

    q.exec(query);

    q.setTableName("last_open", QueryBuilder::Replace);

    q.set("book", bookID);
    q.set("page", pageID);
    q.set("open_date", t);

    q.exec(query);
}

int LibraryBookManager::bookLastPageID(int bookID)
{
    QSqlQuery query(m_db);

    QueryBuilder q;
    q.setTableName("last_open", QueryBuilder::Select);

    q.select("page");
    q.where("book", bookID);

    q.exec(query);

    return query.next() ? query.value(0).toInt() : -1;
}

int LibraryBookManager::getNewBookID()
{
    int bookID =  0;

    do {
        bookID = Utils::Rand::number(111111, 999999);
    } while(m_books.contains(bookID));

    return bookID;
}

