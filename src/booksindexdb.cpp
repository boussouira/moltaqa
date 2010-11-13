#include "booksindexdb.h"
#include "bookslistmodel.h"
#include "importmodel.h"
#include <qsqlquery.h>
#include <qsqlerror.h>
#include <qfile.h>
#include <qdebug.h>

BooksIndexDB::BooksIndexDB()
{
    QSettings settings;
    settings.beginGroup("General");
    m_appDir = settings.value("app_dir").toString();
    m_booksFolder = settings.value("books_folder").toString();
    m_indexDBName = settings.value("index_db").toString();
    settings.endGroup();

    if(QSqlDatabase::contains("BooksListDB")) {
        m_booksListDB = QSqlDatabase::database("BooksListDB");
    } else {
        m_booksListDB = QSqlDatabase::addDatabase("QSQLITE", "BooksListDB");
        m_booksListDB.setDatabaseName(QString("%1/%2")
                                      .arg(m_booksFolder)
                                      .arg(m_indexDBName));
    }

    if (!m_booksListDB.open()) {
        qDebug("[%s:%d] Cannot open database.", __FILE__, __LINE__);
    }
}

QAbstractItemModel *BooksIndexDB::getListModel(bool books)
{
    BooksListModel *model = new BooksListModel();
    BooksListNode *firstNode = new BooksListNode(BooksListNode::Root);
    childCats(firstNode, 0, books);

    model->setRootNode(firstNode);
    return model;
}

void BooksIndexDB::childCats(BooksListNode *parentNode, int pID, bool books)
{
    if(books)
        booksCat(parentNode, pID); // Start with books

    QSqlQuery catQuery(m_booksListDB);
    catQuery.exec(QString("SELECT id, title, catOrder, parentID FROM catList "
                           "WHERE parentID = %1 ORDER BY catOrder").arg(pID));
    while(catQuery.next())
    {
        BooksListNode *catNode = new BooksListNode(BooksListNode::Categorie,
                                                   catQuery.value(1).toString(),
                                                   QString(),
                                                   catQuery.value(0).toInt());
        childCats(catNode, catQuery.value(0).toInt(), books);
        parentNode->appendChild(catNode);
    }
}

void BooksIndexDB::booksCat(BooksListNode *parentNode, int catID)
{
    QSqlQuery bookQuery(m_booksListDB);
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

int BooksIndexDB::getCatIdFromName(const QString &cat)
{
    int count = 0;
    int catID = -1;

    if(cat.isEmpty())
        return catID;

    QSqlQuery bookQuery(m_booksListDB);
    bookQuery.exec(QString("SELECT id FROM catList WHERE title LIKE \"%%1%\" ").arg(cat));
    while(bookQuery.next()) {
        catID = bookQuery.value(0).toInt();
        count++;

    }

    return (count == 1) ? catID : -1;
}

BookInfo *BooksIndexDB::getBookInfo(int bookID)
{
    BookInfo *bookInfo = new BookInfo();
    QSqlQuery bookQuery(m_booksListDB);

    bookQuery.exec(QString("SELECT bookName, bookType, fileName "
                          "From booksList where id = %1 LIMIT 1").arg(bookID));
    if(bookQuery.next()) {
        bookInfo->setBookName(bookQuery.value(0).toString());
        bookInfo->setBookType((BookInfo::Type)bookQuery.value(1).toInt());
        bookInfo->setBookID(bookID);
        bookInfo->setBookPath(QString("%1/%2")
                              .arg(m_booksFolder)
                              .arg(bookQuery.value(2).toString()));
    }

    return bookInfo;
}

int BooksIndexDB::addBook(ImportModelNode *book)
{
    QSqlQuery indexQuery(m_booksListDB);

    QString qurey = QString("INSERT INTO booksList (id, bookID, bookType, bookFlags, bookCat,"
                            "bookName, bookInfo, authorName, authorID, fileName, bookFolder)"
                            "VALUES(NULL, 0, %1, %2, %3, '%4', '%5', '%6', %7, '%8', '')")
            .arg(book->getNodeType())
            .arg(0)
            .arg(book->getCatID())
            .arg(book->getBookName())
            .arg(book->getInfoToolTip())
            .arg(book->getAuthorName())
            .arg(0)
            .arg(book->getBookPath().split("/").last());

    QString newPath = QString("%1/%2")
                      .arg(m_booksFolder)
                      .arg(book->getBookPath().split("/").last());
    if(QFile::copy(book->getBookPath(), newPath)){
        if(QFile::remove(book->getBookPath()))
            qDebug() << "Remove:" << book->getBookPath();
        else
            qWarning() << "Can't remove:" << book->getBookPath();
        return indexQuery.exec(qurey) ? indexQuery.lastInsertId().toInt() : -1;
    } else {
        return -1;
    }
}
