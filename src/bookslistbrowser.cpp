#include "bookslistbrowser.h"
#include "ui_bookslistbrowser.h"
#include "bookslistmodel.h"
#include "bookslistnode.h"
#include "booksindexdb.h"

#include <qdebug.h>
#include <qsqlquery.h>
#include <qsqlerror.h>
#include <qsettings.h>
#include <qevent.h>

BooksListBrowser::BooksListBrowser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BooksListBrowser)
{
    ui->setupUi(this);
    m_infoDB = new BooksIndexDB();
    m_updateList = false;

    showBooksList();
}

BooksListBrowser::~BooksListBrowser()
{
    delete ui;
    delete m_infoDB;
}

void BooksListBrowser::showEvent(QShowEvent* event){
    if(event->type() == QEvent::Show && !event->spontaneous()){
        if(m_updateList){
            showBooksList();
            m_updateList = false;
        }
    }
}

void BooksListBrowser::showBooksList()
{
    ui->treeView->setModel(m_infoDB->getListModel());

    ui->treeView->expandAll();
    ui->treeView->resizeColumnToContents(0);
    ui->treeView->resizeColumnToContents(1);
}

void BooksListBrowser::on_pushButton_clicked()
{
    hide();
}

void BooksListBrowser::on_treeView_doubleClicked(QModelIndex index)
{
    BooksListModel *model = static_cast<BooksListModel*>(ui->treeView->model());
    BooksListNode *node = model->nodeFromIndex(index);
    if(node->getNodeType() == BooksListNode::Book) {
        emit bookSelected(node->getID());
//        accept();
    }
}
