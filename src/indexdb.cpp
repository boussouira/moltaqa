#include "indexdb.h"
#include "libraryinfo.h"
#include "bookinfo.h"
#include "bookexception.h"
#include "bookslistmodel.h"
#include "importmodel.h"
#include "utils.h"

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
    m_connName = "BooksIndexDB";
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

    m_indexDB = QSqlDatabase::addDatabase("QSQLITE", m_connName);
    m_indexDB.setDatabaseName(booksIndexPath);

    if (!m_indexDB.open())
        throw BookException(tr("لم يمكن فتح قاعدة البيانات الأساسية"), m_indexDB.lastError().text());
}

void IndexDB::loadBooksListModel()
{
    m_future = QtConcurrent::run(this, &IndexDB::loadModel);
}

void IndexDB::transaction()
{
    m_indexDB.transaction();
}

bool IndexDB::commit()
{
    return m_indexDB.commit();
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
    int catID = 0;

    if(cat.isEmpty())
        return catID;

    QSqlQuery bookQuery(m_indexDB);
    bookQuery.exec(QString("SELECT id FROM catList WHERE title LIKE \"%%1%\" ").arg(cat));
    while(bookQuery.next()) {
        catID = bookQuery.value(0).toInt();
        count++;

    }

    return (count == 1) ? catID : 0;
}

BookInfo *IndexDB::getBookInfo(int bookID, bool allInfo)
{
    QSqlQuery bookQuery(m_indexDB);

    bookQuery.prepare("SELECT booksList.bookDisplayName, booksList.bookType, booksList.fileName, "
                      "booksList.bookFullName, booksList.bookOtherNames, booksList.bookInfo, "
                      "booksList.bookEdition, booksList.bookPublisher, booksList.bookMohaqeq, "
                      "booksList.authorID, bookMeta.book_info, authorsList.name "
                      "FROM booksList "
                      "LEFT JOIN bookMeta ON bookMeta.id = booksList.id "
                      "LEFT JOIN authorsList ON authorsList.id = booksList.authorID "
                      "WHERE booksList.id = ? LIMIT 1");
    bookQuery.bindValue(0, bookID);

    if(bookQuery.exec()) {
        if(bookQuery.next()) {
            BookInfo *bookInfo = new BookInfo();

            bookInfo->bookDisplayName = bookQuery.value(0).toString();
            bookInfo->bookType = (BookInfo::Type)bookQuery.value(1).toInt();
            bookInfo->bookID = bookID;
            bookInfo->bookPath = m_libraryInfo->bookPath(bookQuery.value(2).toString());

            bookInfo->fromString(bookQuery.value(10).toString());

            if(allInfo) {
                bookInfo->bookFullName = bookQuery.value(3).toString();
                bookInfo->bookOtherNames = bookQuery.value(4).toString();
                bookInfo->bookInfo = bookQuery.value(5).toString();
                bookInfo->bookEdition = bookQuery.value(6).toString();
                bookInfo->bookPublisher = bookQuery.value(7).toString();
                bookInfo->bookMohaqeq = bookQuery.value(8).toString();
                bookInfo->authorID = bookQuery.value(9).toInt();
                bookInfo->authorName = bookQuery.value(11).toString();
            }

            return bookInfo;
        }
    } else {
        LOG_SQL_ERROR(bookQuery);;
    }

    return 0;
}

BookInfo *IndexDB::getQuranBook()
{
    BookInfo *bookInfo = new BookInfo();
    QSqlQuery bookQuery(m_indexDB);

    bookQuery.exec(QString("SELECT booksList.bookDisplayName, booksList.bookType, booksList.fileName, bookMeta.book_info, booksList.id "
                           "FROM booksList LEFT JOIN bookMeta "
                           "ON bookMeta.id = booksList.id "
                           "WHERE booksList.bookType = %1 LIMIT 1").arg(BookInfo::QuranBook));

    if(bookQuery.next()) {
        bookInfo->bookDisplayName = bookQuery.value(0).toString();
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
    QString newBookName = Utils::genBookName(m_libraryInfo->booksDir());
    QString newPath = m_libraryInfo->booksDir() + "/" + newBookName;

    indexQuery.prepare("INSERT INTO booksList (id, bookID, bookType, bookFlags, bookCat, "
                       "bookDisplayName, bookInfo, authorName, authorID, fileName, bookFolder) "
                       "VALUES(NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

    indexQuery.bindValue(0, 0);
    indexQuery.bindValue(1, book->type);
    indexQuery.bindValue(2, 0);
    indexQuery.bindValue(3, book->catID);
    indexQuery.bindValue(4, book->bookName);
    indexQuery.bindValue(5, book->bookInfo);
    indexQuery.bindValue(6, book->authorName);
    indexQuery.bindValue(7, 0);
    indexQuery.bindValue(8, newBookName); // Add file name
    indexQuery.bindValue(9, QVariant(QVariant::String));

    if(QFile::copy(book->bookPath, newPath)){
        if(!QFile::remove(book->bookPath))
            qWarning() << "Can't remove:" << book->bookPath;
        if(indexQuery.exec()) {
            int lastID = indexQuery.lastInsertId().toInt();
            return lastID;
        } else {
            LOG_SQL_ERROR(indexQuery);
            QFile::remove(newPath);
            qDebug() << indexQuery.lastError();
            return -1;
        }
    } else {
        qWarning() << "Can't copy" << book->bookPath << "to" << newPath;
        return -1;
    }
}

void IndexDB::childCats(BooksListNode *parentNode, int pID, bool onlyCats)
{
    if(!onlyCats)
        booksCat(parentNode, pID); // Start with books

    QSqlQuery catQuery(m_indexDB);
    if(!catQuery.exec(QString("SELECT id, title, catOrder, parentID FROM catList "
                           "WHERE parentID = %1 ORDER BY catOrder").arg(pID))) {
        LOG_SQL_ERROR(catQuery);
    }

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
    bookQuery.prepare("SELECT booksList.id, booksList.bookDisplayName, booksList.authorName, booksList.bookInfo, authorsList.name "
                           "FROM booksList LEFT JOIN authorsList "
                           "ON authorsList.id = booksList.authorID "
                           "WHERE booksList.bookCat = ?");
    bookQuery.bindValue(0, catID);
    if(!bookQuery.exec()) {
        LOG_SQL_ERROR(bookQuery);
    }

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
        LOG_SQL_ERROR(bookQuery);
    } else {
        qDebug() << "Meta for" << info->bookID << (newBook?"Inserted":"Updated");
    }
}

void IndexDB::getShoroohPages(BookInfo *info)
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
    bookQuery.bindValue(1, info->currentPageID);
    if(!bookQuery.exec()) {
        LOG_SQL_ERROR(bookQuery);
    }

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
        LOG_SQL_ERROR(bookQuery);
    }
}

void IndexDB::updateCatParent(int catID, int parentID)
{
    QSqlQuery bookQuery(m_indexDB);

    bookQuery.prepare("UPDATE catList SET parentID = ? WHERE id = ?");
    bookQuery.bindValue(0, parentID);
    bookQuery.bindValue(1, catID);
    if(!bookQuery.exec()) {
        LOG_SQL_ERROR(bookQuery);
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
        LOG_SQL_ERROR(bookQuery);
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
        LOG_SQL_ERROR(bookQuery);
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
        LOG_SQL_ERROR(bookQuery);
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
        LOG_SQL_ERROR(bookQuery);
    }

    return 0;
}

void IndexDB::removeCat(int catID)
{
    QSqlQuery bookQuery(m_indexDB);

    bookQuery.prepare("DELETE FROM catList WHERE id = ?");
    bookQuery.bindValue(0, catID);
    if(!bookQuery.exec()) {
        LOG_SQL_ERROR(bookQuery);
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
        LOG_SQL_ERROR(bookQuery);
        return false;
    }
}

QStandardItemModel *IndexDB::getAuthorsListModel()
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

void IndexDB::updateBookInfo(BookInfo *info)
{
    QSqlQuery bookQuery(m_indexDB);
    bookQuery.prepare("UPDATE booksList SET "
                          "bookDisplayName = ?, "
                          "bookFullName = ?, "
                          "bookOtherNames = ?, "
                          "bookInfo = ?, "
                          "bookEdition = ?, "
                          "bookPublisher = ?, "
                          "bookMohaqeq = ?, "
                          "authorID = ? "
                      "WHERE id = ?");
    bookQuery.bindValue(0, info->bookDisplayName);
    bookQuery.bindValue(1, info->bookFullName);
    bookQuery.bindValue(2, info->bookOtherNames);
    bookQuery.bindValue(3, info->bookInfo);
    bookQuery.bindValue(4, info->bookEdition);
    bookQuery.bindValue(5, info->bookPublisher);
    bookQuery.bindValue(6, info->bookMohaqeq);
    bookQuery.bindValue(7, info->authorID);
    bookQuery.bindValue(8, info->bookID);

    if(!bookQuery.exec()) {
        LOG_SQL_ERROR(bookQuery);
    }
}
