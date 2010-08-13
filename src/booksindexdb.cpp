#include "booksindexdb.h"
#include "bookslistmodel.h"
#include <qsqlquery.h>
#include <qsqlerror.h>
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
