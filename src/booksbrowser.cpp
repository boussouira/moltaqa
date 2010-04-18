#include "booksbrowser.h"
#include "ui_booksbrowser.h"

BooksBrowser::BooksBrowser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BooksBrowser)
{
    ui->setupUi(this);

    loadBooks();
}

BooksBrowser::~BooksBrowser()
{
    delete ui;
}

void BooksBrowser::loadBooks()
{
    QSqlDatabase booksLib = QSqlDatabase::addDatabase("QSQLITE", "BookLib");
    booksLib.setDatabaseName("books/books_index.db");

    if (!booksLib.open()) {
        qDebug() << "Cannot open database.";
    }
    QSqlQuery *bookQuery = new QSqlQuery(booksLib);
    QSqlQuery *catQuery = new QSqlQuery(booksLib);

//    BooksListNode *rootNode = new BooksListNode(BooksListNode::Root);
    BooksListNode *firstNode = new BooksListNode(BooksListNode::Root, trUtf8("الفهرس"));

//    rootNode->appendChild(firstNode);
    catQuery->exec("SELECT id, name, catord, level FROM books_cats ORDER BY catord");

    while(catQuery->next())
    {
        int bookcount = 0;
        int tid = catQuery->value(0).toInt();
        int level = catQuery->value(3).toInt();
        BooksListNode *firstChild = new BooksListNode(BooksListNode::Categorie,
                                                      catQuery->value(1).toString());
        BooksListNode *parent = getNodeByDepth(firstNode, level+1);

        bookQuery->exec(QString("SELECT book_name, auth FROM books_list WHERE parent_cat_id = %1 ")
                        .arg(tid));
        while(bookQuery->next())
        {
            BooksListNode *secondChild = new BooksListNode(BooksListNode::Book,
                                         bookQuery->value(0).toString(),
                                         bookQuery->value(1).toString());
            BooksListNode *parent2 = getNodeByDepth(firstChild, 1);
            parent2->appendChild(secondChild);

            bookcount++;
        }

        if(bookcount > 0) {
            firstChild->setId(tid);
            parent->appendChild(firstChild);
        }

    }

    BooksListModel *booleanModel = new BooksListModel();
//    booleanModel->setRootNode(rootNode);
    booleanModel->setRootNode(firstNode);
    ui->treeView->setModel(booleanModel);
    ui->treeView->expandAll();
    ui->treeView->resizeColumnToContents(0);
    ui->treeView->resizeColumnToContents(1);
    ui->treeView->setSortingEnabled(true);
}

BooksListNode *BooksBrowser::getNodeByDepth(BooksListNode *pNode, int pDepth)
{
    BooksListNode *n = pNode;

    while(--pDepth > 0) {
        n = n->children.last();
    }
    return n;
}
