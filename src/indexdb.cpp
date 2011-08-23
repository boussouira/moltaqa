#include "indexdb.h"
#include "libraryinfo.h"
#include "bookinfo.h"
#include "bookexception.h"
#include "bookslistmodel.h"
#include "importmodel.h"
#include "common.h"

#include <qdebug.h>
#include <qsqlquery.h>
#include <qfile.h>
#include <qdir.h>
#include <qsqlerror.h>
#include <qdatetime.h>

IndexDB::IndexDB(LibraryInfo *info)
{
    m_libraryInfo = info;
    m_model = new BooksListModel();
}

IndexDB::~IndexDB()
{
    if(m_future.isRunning())
        m_future.waitForFinished();
}

void IndexDB::setConnectionInfo(LibraryInfo *info)
{
    m_libraryInfo = info;
}

LibraryInfo *IndexDB::connectionInfo()
{
    return m_libraryInfo;
}

void IndexDB::open()
{
    QString booksIndexPath = m_libraryInfo->booksIndexPath();

    if(!QFile::exists(booksIndexPath))
       qWarning("Can't find index database: %s", qPrintable(booksIndexPath));

    m_indexDB = QSqlDatabase::addDatabase("QSQLITE", "BooksIndexDB");
    m_indexDB.setDatabaseName(booksIndexPath);

    if (!m_indexDB.open())
        throw BookException(tr("لم يمكن فتح قاعدة البيانات الأساسية"), m_indexDB.lastError().text());
}

void IndexDB::loadBooksListModel()
{
    m_future = QtConcurrent::run(this, &IndexDB::loadModel);
}

void IndexDB::loadModel()
{
    BooksListNode *firstNode = new BooksListNode(BooksListNode::Root);
    childCats(firstNode, 0, false);

    m_model->setRootNode(firstNode);
}

BooksListModel *IndexDB::booksListModel()
{
    if(m_future.isRunning())
        m_future.waitForFinished();

    return m_model;
}

BooksListModel *IndexDB::catsListModel()
{
    BooksListModel *model = new BooksListModel();
    BooksListNode *firstNode = new BooksListNode(BooksListNode::Root);
    childCats(firstNode, 0, true);

    model->setRootNode(firstNode);

    return model;
}

int IndexDB::catIdFromName(const QString &cat)
{
    int count = 0;
    int catID = -1;

    if(cat.isEmpty())
        return catID;

    QSqlQuery bookQuery(m_indexDB);
    bookQuery.exec(QString("SELECT id FROM catList WHERE title LIKE \"%%1%\" ").arg(cat));
    while(bookQuery.next()) {
        catID = bookQuery.value(0).toInt();
        count++;

    }

    return (count == 1) ? catID : -1;
}

BookInfo *IndexDB::getBookInfo(int bookID)
{
    BookInfo *bookInfo = new BookInfo();
    QSqlQuery bookQuery(m_indexDB);

    bookQuery.exec(QString("SELECT booksList.bookName, booksList.bookType, booksList.fileName, bookMeta.book_info "
                           "FROM booksList LEFT JOIN bookMeta "
                           "ON bookMeta.id = booksList.id "
                           "WHERE booksList.id = %1 LIMIT 1").arg(bookID));

    if(bookQuery.next()) {
        bookInfo->bookName = bookQuery.value(0).toString();
        bookInfo->bookType = (BookInfo::Type)bookQuery.value(1).toInt();
        bookInfo->bookID = bookID;
        bookInfo->bookPath = m_libraryInfo->bookPath(bookQuery.value(2).toString());

        bookInfo->fromString(bookQuery.value(3).toString());
    }

    return bookInfo;
}

BookInfo *IndexDB::getQuranBook()
{
    BookInfo *bookInfo = new BookInfo();
    QSqlQuery bookQuery(m_indexDB);

    bookQuery.exec(QString("SELECT booksList.bookName, booksList.bookType, booksList.fileName, bookMeta.book_info, booksList.id "
                           "FROM booksList LEFT JOIN bookMeta "
                           "ON bookMeta.id = booksList.id "
                           "WHERE booksList.bookType = %1 LIMIT 1").arg(BookInfo::QuranBook));

    if(bookQuery.next()) {
        bookInfo->bookName = bookQuery.value(0).toString();
        bookInfo->bookType = BookInfo::QuranBook;
        bookInfo->bookID = bookQuery.value(4).toInt();
        bookInfo->bookPath = m_libraryInfo->bookPath(bookQuery.value(2).toString());

        bookInfo->fromString(bookQuery.value(3).toString());
    }

    return bookInfo;
}

QList<QPair<int, QString> > IndexDB::getTafassirList()
{
    QList<QPair<int, QString> > list;
    QSqlQuery bookQuery(m_indexDB);

    bookQuery.exec("SELECT book_id, tafessir_name FROM tafassirList");
    while(bookQuery.next()) {
        list.append(qMakePair(bookQuery.value(0).toInt(), bookQuery.value(1).toString()));
    }

    return list;
}

int IndexDB::addBook(ImportModelNode *book)
{
    QSqlQuery indexQuery(m_indexDB);
    QString newBookName = genBookName(m_libraryInfo->booksDir());
    QString newPath = m_libraryInfo->booksDir() + "/" + newBookName;

    indexQuery.prepare("INSERT INTO booksList (id, bookID, bookType, bookFlags, bookCat,"
                       "bookName, bookInfo, authorName, authorID, fileName, bookFolder)"
                       "VALUES(NULL, :book_id, :book_type, :book_flags, :cat_id, :book_name, "
                       ":book_info, :author_name, :author_id, :file_name, :book_folder)");

    indexQuery.bindValue(":book_id", 0);
    indexQuery.bindValue(":book_type", book->nodeType());
    indexQuery.bindValue(":book_flags", 0);
    indexQuery.bindValue(":cat_id", book->catID());
    indexQuery.bindValue(":book_name", book->bookName());
    indexQuery.bindValue(":book_info", book->bookInfo());
    indexQuery.bindValue(":author_name", book->authorName());
    indexQuery.bindValue(":author_id", 0);
    indexQuery.bindValue(":file_name", newBookName); // Add file name
    indexQuery.bindValue(":book_folder", QVariant(QVariant::String));

    if(QFile::copy(book->bookPath(), newPath)){
        if(!QFile::remove(book->bookPath()))
            qWarning() << "Can't remove:" << book->bookPath();
        if(indexQuery.exec()) {
            int lastID = indexQuery.lastInsertId().toInt();
            return lastID;
        } else {
            qWarning() << indexQuery.lastError().text();
            return -1;
        }
    } else {
        qWarning() << "Can't copy" << book->bookPath() << "to" << newPath;
        return -1;
    }
}

void IndexDB::childCats(BooksListNode *parentNode, int pID, bool onlyCats)
{
    if(!onlyCats)
        booksCat(parentNode, pID); // Start with books

    QSqlQuery catQuery(m_indexDB);
    catQuery.exec(QString("SELECT id, title, catOrder, parentID FROM catList "
                           "WHERE parentID = %1 ORDER BY catOrder").arg(pID));
    while(catQuery.next())
    {
        BooksListNode *catNode = new BooksListNode(BooksListNode::Categorie,
                                                   catQuery.value(1).toString(),
                                                   QString(),
                                                   catQuery.value(0).toInt());
        childCats(catNode, catQuery.value(0).toInt(), onlyCats);
        parentNode->appendChild(catNode);
    }
}

void IndexDB::booksCat(BooksListNode *parentNode, int catID)
{
    QSqlQuery bookQuery(m_indexDB);
    bookQuery.prepare("SELECT booksList.id, booksList.bookName, booksList.authorName, booksList.bookInfo, authorsList.name "
                           "FROM booksList LEFT JOIN authorsList "
                           "ON authorsList.id = booksList.authorID "
                           "WHERE booksList.bookCat = ?");
    bookQuery.bindValue(0, catID);
    bookQuery.exec();

    while(bookQuery.next())
    {
        BooksListNode *secondChild = new BooksListNode(BooksListNode::Book,
                                                       bookQuery.value(1).toString(),
                                                       bookQuery.value(4).toString(),
                                                       bookQuery.value(0).toInt());
        secondChild->infoToolTip = bookQuery.value(3).toString();
        parentNode->appendChild(secondChild);
    }
}

void IndexDB::updateBookMeta(BookInfo *info, bool newBook)
{
    QSqlQuery bookQuery(m_indexDB);
    if(newBook) {
        bookQuery.prepare("INSERT INTO bookMeta (id, book_info, add_time, update_time) VALUES (?, ?, ?, ?)");
        bookQuery.bindValue(0, info->bookID);
        bookQuery.bindValue(1, info->toString());
        bookQuery.bindValue(2, QDateTime::currentDateTime().toTime_t());
        bookQuery.bindValue(3, QDateTime::currentDateTime().toTime_t());
    } else {
        bookQuery.prepare("INSERT INTO bookMeta (id, book_info, update_time) VALUES (?, ?, ?)");
        bookQuery.bindValue(0, info->bookID);
        bookQuery.bindValue(1, info->toString());
        bookQuery.bindValue(2, QDateTime::currentDateTime().toTime_t());
    }

    if(!bookQuery.exec()) {
        qDebug() << "Error:" << bookQuery.lastError().text() << bookQuery.executedQuery();
    } else {
        qDebug() << "Meta for" << info->bookID << (newBook?"Inserted":"Updated");
    }
}

void IndexDB::getShoroohPages(BookInfo *info)
{
    qDeleteAll(info->shorooh);
    info->shorooh.clear();

    QSqlQuery bookQuery(m_indexDB);

    bookQuery.prepare("SELECT ShareehMeta.shareeh_book, ShareehMeta.shareeh_id, booksList.bookName "
                      "FROM ShareehMeta "
                      "LEFT JOIN booksList "
                      "ON booksList.id =  ShareehMeta.shareeh_book "
                      "WHERE ShareehMeta.mateen_book = ? AND ShareehMeta.mateen_id = ?");
    bookQuery.bindValue(0, info->bookID);
    bookQuery.bindValue(1, info->currentPageID);
    bookQuery.exec();

    while(bookQuery.next()) {
        info->shorooh.append(new BookShorooh(bookQuery.value(0).toInt(),
                                             bookQuery.value(1).toInt(),
                                             bookQuery.value(2).toString()));
    }
}

void IndexDB::updateCatTitle(int catID, QString title)
{
    QSqlQuery bookQuery(m_indexDB);

    bookQuery.prepare("UPDATE catList SET title = ? WHERE id = ?");
    bookQuery.bindValue(0, title);
    bookQuery.bindValue(1, catID);
    if(!bookQuery.exec()) {
        SQL_ERROR(bookQuery.lastError().text());
    }
}

void IndexDB::updateCatParent(int catID, int parentID)
{
    QSqlQuery bookQuery(m_indexDB);

    bookQuery.prepare("UPDATE catList SET parentID = ? WHERE id = ?");
    bookQuery.bindValue(0, parentID);
    bookQuery.bindValue(1, catID);
    if(!bookQuery.exec()) {
        SQL_ERROR(bookQuery.lastError().text());
    }
}

void IndexDB::updateCatOrder(int catID, int catOrder)
{
    QSqlQuery bookQuery(m_indexDB);
    qDebug("SET CAT ORDER = %d FOR CAT = %d", catOrder, catID);

    bookQuery.prepare("UPDATE catList SET catOrder = ? WHERE id = ?");
    bookQuery.bindValue(0, catOrder);
    bookQuery.bindValue(1, catID);
    if(!bookQuery.exec()) {
        SQL_ERROR(bookQuery.lastError().text());
    }
}

void IndexDB::makeCatPlace(int parentID, int catOrder)
{
    QSqlQuery bookQuery(m_indexDB);
    qDebug("Make place for order %d", catOrder);

    bookQuery.prepare("UPDATE catList SET catOrder = catOrder + 1 WHERE catOrder >= ? AND parentID = ?");
    bookQuery.bindValue(0, catOrder);
    bookQuery.bindValue(1, parentID);
    if(!bookQuery.exec()) {
        SQL_ERROR(bookQuery.lastError().text());
    }
}

int IndexDB::booksCount(int catID)
{
    QSqlQuery bookQuery(m_indexDB);

    bookQuery.prepare("SELECT COUNT(*) FROM booksList WHERE bookCat = ?");
    bookQuery.bindValue(0, catID);
    if(bookQuery.exec()) {
        if(bookQuery.next()) {
            return bookQuery.value(0).toInt();
        }
    } else {
        SQL_ERROR(bookQuery.lastError().text());
    }

    return 0;
}

int IndexDB::addNewCat(const QString &title)
{
    QSqlQuery bookQuery(m_indexDB);

    bookQuery.prepare("INSERT INTO catList (id, title) VALUES (NULL, ?)");
    bookQuery.bindValue(0, title);

    if(bookQuery.exec()) {
        return bookQuery.lastInsertId().toInt();
    } else {
        SQL_ERROR(bookQuery.lastError().text());
    }

    return 0;
}

void IndexDB::removeCat(int catID)
{
    QSqlQuery bookQuery(m_indexDB);

    bookQuery.prepare("DELETE FROM catList WHERE id = ?");
    bookQuery.bindValue(0, catID);
    if(!bookQuery.exec()) {
        SQL_ERROR(bookQuery.lastError().text());
    }
}

bool IndexDB::moveCatBooks(int fromCat, int toCat)
{
    QSqlQuery bookQuery(m_indexDB);

    bookQuery.prepare("UPDATE booksList SET bookCat = ? WHERE bookCat = ?");
    bookQuery.bindValue(0, toCat);
    bookQuery.bindValue(1, fromCat);
    if(bookQuery.exec()) {
        return true;
    } else {
        SQL_ERROR(bookQuery.lastError().text());
        return false;
    }
}
