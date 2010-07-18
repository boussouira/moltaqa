#include "bookslistbrowser.h"
#include "ui_bookslistbrowser.h"
#include "bookslistmodel.h"
#include "bookslistnode.h"

#include <qdebug.h>
#include <qsqlquery.h>
#include <qsqlerror.h>
#include <qsettings.h>

BooksListBrowser::BooksListBrowser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BooksListBrowser)
{
    ui->setupUi(this);
    m_listModel = new BooksListModel();

    QSettings settings;
    m_appDir = settings.value("General/app_dir").toString();

    showBooksList();
}

BooksListBrowser::~BooksListBrowser()
{
    delete ui;
    delete m_listModel;
}

void BooksListBrowser::showBooksList()
{
    if(QSqlDatabase::contains("BooksListDB")) {
        m_booksListDB = QSqlDatabase::database("BooksListDB");
    } else {
        m_booksListDB = QSqlDatabase::addDatabase("QSQLITE", "BooksListDB");
        m_booksListDB.setDatabaseName(QString("%1/books/books_index.db").arg(m_appDir));
    }

    if (!m_booksListDB.open()) {
        qDebug("[%s:%d] Cannot open database.", __FILE__, __LINE__);
    }

    BooksListNode *firstNode = new BooksListNode(BooksListNode::Root);
    childCats(firstNode, 0);

    m_listModel->setRootNode(firstNode);
    ui->treeView->setModel(m_listModel);
    ui->treeView->expandAll();
    ui->treeView->resizeColumnToContents(0);
    ui->treeView->resizeColumnToContents(1);
}

void BooksListBrowser::childCats(BooksListNode *parentNode, int pID)
{
    booksCat(parentNode, pID); // Start with books
    QSqlQuery *catQuery = new QSqlQuery(m_booksListDB);
    catQuery->exec(QString("SELECT id, title, catOrder, parentID FROM catList "
                           "WHERE parentID = %1 ORDER BY catOrder").arg(pID));
    while(catQuery->next())
    {
        BooksListNode *catNode = new BooksListNode(BooksListNode::Categorie,
                                                   catQuery->value(1).toString(),
                                                   QString(),
                                                   catQuery->value(0).toInt());
        childCats(catNode, catQuery->value(0).toInt());
        parentNode->appendChild(catNode);
    }
}

void BooksListBrowser::booksCat(BooksListNode *parentNode, int catID)
{
    QSqlQuery *bookQuery = new QSqlQuery(m_booksListDB);
    bookQuery->exec(QString("SELECT id, bookName, authorName, bookInfo FROM booksList "
                            "WHERE bookCat = %1 ").arg(catID));
    while(bookQuery->next())
    {
        BooksListNode *secondChild = new BooksListNode(BooksListNode::Book,
                                                       bookQuery->value(1).toString(),
                                                       bookQuery->value(2).toString(),
                                                       bookQuery->value(0).toInt());
        secondChild->setInfoToolTip(bookQuery->value(3).toString());
        parentNode->appendChild(secondChild);
    }
}

void BooksListBrowser::on_pushButton_clicked()
{
    hide();
}

void BooksListBrowser::on_treeView_doubleClicked(QModelIndex index)
{
    BooksListNode *node = m_listModel->nodeFromIndex(index);
    if(node->getNodeType() == BooksListNode::Book) {
        emit bookSelected(node->getID());
//        accept();
    }
}
