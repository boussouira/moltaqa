#include "librarybookmanager.h"
#include "mainwindow.h"
#include "librarymanager.h"
#include "libraryinfo.h"
#include "modelenums.h"
#include "utils.h"
#include "xmlutils.h"
#include "timeutils.h"
#include "authorsmanager.h"
#include "bookreaderview.h"

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
    m_uuid.clear();
}

StandardItemModelPtr LibraryBookManager::getModel(bool bookIcon)
{
    QStandardItemModel *model = new QStandardItemModel();

    QSqlQuery query(m_db);
    query.prepare("SELECT id, title, authorID FROM books ORDER BY id");

    ml_query_exec(query);

    while(query.next()) {
        QList<QStandardItem*> rows;
        QStandardItem *item = new QStandardItem();
        item->setText(query.value(1).toString());
        item->setData(query.value(0).toInt(), ItemRole::idRole);

        if(bookIcon)
            item->setIcon(QIcon(":/images/book.png"));

        rows << item;

        AuthorInfo::Ptr author = m_authorsManager->getAuthorInfo(query.value(2).toInt());
        if(author) {
            QStandardItem *authorItem = new QStandardItem();
            authorItem->setText(AuthorInfo::formatAuthorName(author));
            authorItem->setData(author->id, ItemRole::authorIdRole);
            authorItem->setData(author->deathYear, ItemRole::authorDeathRole);

            rows << authorItem;
        }

        model->appendRow(rows);
    }

    model->setHorizontalHeaderLabels(QStringList() << tr("الكتب") << tr("المؤلف"));

    return StandardItemModelPtr(model);
}

StandardItemModelPtr LibraryBookManager::getLastOpendModel()
{
    QStandardItemModel *model = new QStandardItemModel();

    QSqlQuery query(m_db);//    0              1                 2                 3
    query.prepare("SELECT last_open.book, last_open.page, last_open.open_date, books.title "
                  "FROM last_open "
                  "LEFT JOIN books "
                  "ON books.id = last_open.book "
                  "ORDER BY open_date DESC");

    ml_query_exec(query);

    while(query.next()) {
        QStandardItem *item = new QStandardItem();
        item->setText(query.value(3).toString());
        item->setData(query.value(1).toInt(), ItemRole::pageIdRole);
        item->setData(query.value(0).toInt(), ItemRole::idRole);
        item->setData(ItemType::BookItem, ItemRole::itemTypeRole);
        item->setIcon(QIcon(":/images/book.png"));

        QStandardItem *timeItem = new QStandardItem();
        timeItem->setText(Utils::Time::elapsedTime(query.value(2).toInt()));

        model->invisibleRootItem()->appendRow(QList<QStandardItem*>() << item << timeItem);
    }

    model->setHorizontalHeaderLabels(QStringList() << tr("الكتب") << tr("اخر تصفح قبل"));

    return StandardItemModelPtr(model);
}

StandardItemModelPtr LibraryBookManager::getBookHistoryModel(int bookID)
{
    QStandardItemModel *model = new QStandardItemModel();

    QSqlQuery query(m_db);//0   1
    query.prepare("SELECT page, open_date "
                  "FROM history "
                  "WHERE book = ? "
                  "ORDER BY open_date DESC");
    query.bindValue(0, bookID);

    ml_query_exec(query);

    while(query.next()) {
        QStandardItem *item = new QStandardItem();
        item->setText(tr("الصفحة %1").arg(query.value(0).toInt()));
        item->setData(query.value(0).toInt(), ItemRole::idRole);

        QStandardItem *timeItem = new QStandardItem();
        timeItem->setText(Utils::Time::elapsedTime(query.value(1).toInt()));

        model->invisibleRootItem()->appendRow(QList<QStandardItem*>() << item << timeItem);
    }

    model->setHorizontalHeaderLabels(QStringList() << tr("الصفحة") << tr("التاريخ"));

    return StandardItemModelPtr(model);
}

LibraryBook::Ptr LibraryBookManager::getLibraryBook(int bookID)
{
    LibraryBook::Ptr book = m_books.value(bookID);
    if(book)
        return book;

    QSqlQuery query(m_db);
    query.prepare("SELECT id, uuid, title, type, authorID, author, info, bookFlags, indexFlags, filename, "
                  "otherTitles, edition, publisher, mohaqeq, comment "
                  "FROM books WHERE id = ?");
    query.bindValue(0, bookID);

    ml_query_exec(query);

    if(query.next()) {
        LibraryBook::Ptr book(new LibraryBook());
        book->id = query.value(0).toInt();
        book->uuid = query.value(1).toString();
        book->title = query.value(2).toString();
        book->type = static_cast<LibraryBook::Type>(query.value(3).toInt());
        book->info = query.value(6).toString();
        book->comment = query.value(14).toString();

        book->otherTitles = query.value(10).toString();
        book->edition = query.value(11).toString();
        book->publisher = query.value(12).toString();
        book->mohaqeq = query.value(13).toString();

        book->bookFlags = static_cast<LibraryBook::BookFlags>(query.value(7).toInt());
        book->indexFlags = static_cast<LibraryBook::IndexFlags>(query.value(8).toInt());

        book->fileName = query.value(9).toString();
        book->path = m_libraryInfo->bookPath(book->fileName);

        if(!book->isQuran()) {
            book->authorID = query.value(4).toInt();

            if(book->authorID) {
                book->authorName = m_authorsManager->getAuthorName(book->authorID);
            } else {
                book->authorName = query.value(5).toString();
            }
        }

        m_books.insert(book->id, book);

        return book;
    }

    return LibraryBook::Ptr();
}

LibraryBook::Ptr LibraryBookManager::getLibraryBook(QString bookUUID)
{
    if(m_uuid.contains(bookUUID))
        return getLibraryBook(m_uuid[bookUUID]);

    QSqlQuery query(m_db);
    query.prepare("SELECT id FROM books WHERE uuid = ?");
    query.bindValue(0, bookUUID);

    ml_query_exec(query);

    if(query.next()) {
        int bookID = query.value(0).toInt();
        m_uuid[bookUUID] = bookID;

        return getLibraryBook(bookID);
    }

    return LibraryBook::Ptr();
}

LibraryBook::Ptr LibraryBookManager::getQuranBook()
{
    if(m_quranBook)
        return m_quranBook;

    QSqlQuery query(m_db);
    query.prepare("SELECT id FROM books WHERE type = ?");
    query.bindValue(0, LibraryBook::QuranBook);

    ml_query_exec(query);

    if(query.next()) {
        m_quranBook = getLibraryBook(query.value(0).toInt());
        if(m_quranBook)
            return m_quranBook;
    }

    return LibraryBook::Ptr();
}

LibraryBook::Ptr LibraryBookManager::findBook(QString bookName)
{
    QueryBuilder q;
    q.setTableName("books", QueryBuilder::Select);
    q.select("id");
    q.where("title", bookName);

    QSqlQuery query(m_db);
    if(q.exec(query) && query.next())
        return getLibraryBook(query.value(0).toInt());

    return LibraryBook::Ptr();
}

int LibraryBookManager::booksCount()
{
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM books");
    if(query.exec() && query.next())
        return query.value(0).toInt();

    return 0;
}

int LibraryBookManager::addBook(LibraryBook::Ptr book)
{
    QMutexLocker locker(&m_mutex);

    if(!book->id || getLibraryBook(book->id))
        book->id = getNewBookID();

    if(book->uuid.isEmpty() || getLibraryBook(book->uuid))
        book->uuid = getNewBookUUID();

    QSqlQuery query(m_db);

    QueryBuilder q;
    q.setTableName("books");
    q.setQueryType(QueryBuilder::Insert);

    q.set("id", book->id);
    q.set("uuid", book->uuid);
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

    ml_return_val_on_fail(q.exec(query), 0);

    return book->id;
}

bool LibraryBookManager::updateBook(LibraryBook::Ptr book)
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

    ml_return_val_on_fail(q.exec(query), false);

    m_books.insert(book->id, book);
    return true;
}

bool LibraryBookManager::removeBook(int bookID)
{
    LibraryBook::Ptr book = getLibraryBook(bookID);
    ml_return_val_on_fail2(book, "LibraryBookManager::removeBook can't find book" << bookID, false);

    QSqlQuery query(m_db);
    query.prepare("DELETE FROM books WHERE id = ?");
    query.bindValue(0, bookID);
    if(!query.exec()) {
        ml_warn_query_error(query);
        return false;
    }

    MW->bookReaderView()->bookWidgetManager()->closeBook(bookID);

    if(book->id != LibraryManager::helpBookID()) {
        ml_warn_on_fail(QFile::remove(book->path),
                        "LibraryBookManager::removeBook can't remove file" << book->path);
    }

    m_books.remove(bookID);

    deleteBookFromLastOpen(bookID);
    deleteBookHistory(bookID);

    return true;
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
        ml_warn_query_error(query);
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

    if(bookID != -1)
        q.where("id", bookID);

    ml_return_on_fail(q.exec(query));

    if(bookID != -1) {
        LibraryBook::Ptr book = m_books.value(bookID);
        ml_return_on_fail2(book, "LibraryBookManager::setBookIndexStat No book with id" << bookID << "where found");

        book->indexFlags = indexFlag;
    }
}

QList<LibraryBook::Ptr> LibraryBookManager::getAuthorBooks(int authorID)
{
    QList<LibraryBook::Ptr> list;

    QSqlQuery query(m_db);
    query.prepare("SELECT id FROM books WHERE authorID = ?");
    query.bindValue(0, authorID);

    ml_query_exec(query);

    while(query.next()) {
        LibraryBook::Ptr book = getLibraryBook(query.value(0).toInt());
        if(book)
            list.append(book);
    }

    return list;
}

void LibraryBookManager::addBookHistory(int bookID, int pageID)
{
    QSqlQuery query(m_db);

    uint t = QDateTime::currentDateTime().toTime_t();

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

bool LibraryBookManager::deleteBookHistory(int bookID)
{
    QueryBuilder q;
    q.setTableName("history", QueryBuilder::Delete);

    if(bookID != -1)
        q.where("book", bookID);

    return q.exec(m_db);
}

bool LibraryBookManager::deleteBookFromLastOpen(int bookID)
{
    QueryBuilder q;
    q.setTableName("last_open", QueryBuilder::Delete);

    if(bookID != -1)
        q.where("book", bookID);

    return q.exec(m_db);
}

int LibraryBookManager::getNewBookID()
{
    int bookID =  0;

    do {
        bookID = Utils::Rand::number(111111, 999999);
    } while(getLibraryBook(bookID));

    return bookID;
}

QString LibraryBookManager::getNewBookUUID()
{
    QString bookUUID;

    do {
        bookUUID = Utils::Rand::uuid();
    } while(getLibraryBook(bookUUID));

    return bookUUID;
}

