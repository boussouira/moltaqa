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

IndexDB::IndexDB()
{
}

IndexDB::IndexDB(LibraryInfo *info)
{
    m_libraryInfo = info;
}

IndexDB::~IndexDB()
{
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

QAbstractItemModel *IndexDB::booksList(bool onlyCats)
{
    BooksListModel *model = new BooksListModel();
    BooksListNode *firstNode = new BooksListNode(BooksListNode::Root);
    childCats(firstNode, 0, onlyCats);

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
    bookQuery.exec(QString("SELECT id, bookName, authorName, bookInfo FROM booksList "
                            "WHERE bookCat = %1 ").arg(catID));
    while(bookQuery.next())
    {
        BooksListNode *secondChild = new BooksListNode(BooksListNode::Book,
                                                       bookQuery.value(1).toString(),
                                                       bookQuery.value(2).toString(),
                                                       bookQuery.value(0).toInt());
        secondChild->setInfoToolTip(bookQuery.value(3).toString());
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
