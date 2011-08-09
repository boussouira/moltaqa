#include "bookslistbrowser.h"
#include "ui_bookslistbrowser.h"
#include "bookslistmodel.h"
#include "bookslistnode.h"
#include "indexdb.h"

#include <qsqlquery.h>
#include <qsqlerror.h>
#include <qsettings.h>
#include <qevent.h>
#include <qsettings.h>
#include "sortfilterproxymodel.h"

BooksListBrowser::BooksListBrowser(IndexDB *indexDB, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BooksListBrowser)
{
    ui->setupUi(this);

    loadSettings();
    m_indexDB = indexDB;
    m_updateList = true;

    m_filterModel = new SortFilterProxyModel(this);
    connect(ui->lineSearch, SIGNAL(textChanged(QString)), m_filterModel, SLOT(setFilterRegExp(QString)));// TODO: serach in book info...
    connect(ui->lineSearch, SIGNAL(textChanged(QString)), ui->treeView, SLOT(expandAll()));
}

BooksListBrowser::~BooksListBrowser()
{
    delete ui;
}

void BooksListBrowser::showEvent(QShowEvent* event){
    if(event->type() == QEvent::Show && !event->spontaneous()){
        if(m_updateList){
            loadBooksList();
            m_updateList = false;
        }
    }
}

void BooksListBrowser::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.beginGroup("BooksListWidget");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.endGroup();

    event->accept();
}

void BooksListBrowser::loadSettings()
{
    QSettings settings;
    settings.beginGroup("BooksListWidget");
    move(settings.value("pos", pos()).toPoint());
    resize(settings.value("size", size()).toSize());
    settings.endGroup();
}

void BooksListBrowser::loadBooksList()
{
    m_filterModel->setSourceModel(m_indexDB->booksList());
    m_filterModel->setFilterKeyColumn(0);

    ui->treeView->setModel(m_filterModel);

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
    BooksListNode *node = model->nodeFromIndex(m_filterModel->mapToSource(index));
    if(node->getNodeType() == BooksListNode::Book) {
        emit bookSelected(node->getID());
//        accept();
    }
}
