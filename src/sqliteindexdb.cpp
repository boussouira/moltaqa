#include "sqliteindexdb.h"
#include "libraryinfo.h"
#include "bookinfo.h"
#include "bookexception.h"
#include "bookslistmodel.h"
#include "importmodel.h"

#include <qdebug.h>
#include <qsqlquery.h>
#include <qfile.h>
#include <qdir.h>
#include <qsqlerror.h>

SqliteIndexDB::SqliteIndexDB()
{
}

SqliteIndexDB::SqliteIndexDB(LibraryInfo *info) : IndexDB(info)
{
}

SqliteIndexDB::~SqliteIndexDB()
{
}

void SqliteIndexDB::open()
{
    QString booksIndexPath = m_libraryInfo->booksIndexPath();
    if(!QFile::exists(booksIndexPath))
        throw BookException(tr("لم يتم العثور على قاعدة البيانات الأساسية"), m_libraryInfo->path());

    if(QSqlDatabase::contains(m_libraryInfo->connectionName())) {
        m_indexDB = QSqlDatabase::database(m_libraryInfo->connectionName());
    } else {
        m_indexDB = QSqlDatabase::addDatabase(m_libraryInfo->driverName(),
                                                  m_libraryInfo->connectionName());
        m_indexDB.setDatabaseName(booksIndexPath);
    }

    if (!m_indexDB.open())
        throw BookException(tr("لم يمكن فتح قاعدة البيانات الأساسية"), booksIndexPath);
}

QAbstractItemModel *SqliteIndexDB::booksList(bool onlyCats)
{
    BooksListModel *model = new BooksListModel();
    BooksListNode *firstNode = new BooksListNode(BooksListNode::Root);
    childCats(firstNode, 0, onlyCats);

    model->setRootNode(firstNode);
    return model;
}

int SqliteIndexDB::catIdFromName(const QString &cat)
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

BookInfo *SqliteIndexDB::getBookInfo(int bookID)
{
    BookInfo *bookInfo = new BookInfo();
    QSqlQuery bookQuery(m_indexDB);

    bookQuery.exec(QString("SELECT bookName, bookType, fileName "
                          "From booksList where id = %1 LIMIT 1").arg(bookID));
    if(bookQuery.next()) {
        bookInfo->setBookName(bookQuery.value(0).toString());
        bookInfo->setBookType((BookInfo::Type)bookQuery.value(1).toInt());
        bookInfo->setBookID(bookID);
        bookInfo->setBookPath(m_libraryInfo->bookPath(bookQuery.value(2).toString()));
    }

    return bookInfo;
}

int SqliteIndexDB::addBook(ImportModelNode *book)
{
    QSqlQuery indexQuery(m_indexDB);

    QString qurey = QString("INSERT INTO booksList (id, bookID, bookType, bookFlags, bookCat,"
                            "bookName, bookInfo, authorName, authorID, fileName, bookFolder)"
                            "VALUES(NULL, 0, %1, %2, %3, '%4', '%5', '%6', %7, '%8', '')")
            .arg(book->nodeType())
            .arg(0)
            .arg(book->catID())
            .arg(book->bookName())
            .arg(book->bookInfo())
            .arg(book->authorName())
            .arg(0)
            .arg(book->bookPath().split("/").last());

    QString newPath = m_libraryInfo->bookPath(book->bookPath().split("/").last());

    if(QFile::copy(book->bookPath(), newPath)){
        if(!QFile::remove(book->bookPath()))
            qWarning() << "Can't remove:" << book->bookPath();
        if(indexQuery.exec(qurey)) {
            qWarning() << indexQuery.lastError().text();
            return indexQuery.lastInsertId().toInt();
        } else {
            return -1;
        }
    } else {
        qWarning() << "Can't copy" << book->bookPath() << "to" << newPath;
        return -1;
    }
}

void SqliteIndexDB::childCats(BooksListNode *parentNode, int pID, bool onlyCats)
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

void SqliteIndexDB::booksCat(BooksListNode *parentNode, int catID)
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
