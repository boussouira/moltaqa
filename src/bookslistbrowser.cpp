#include "bookslistbrowser.h"
#include "ui_bookslistbrowser.h"
#include "bookslistmodel.h"
#include "bookslistnode.h"
#include "librarymanager.h"
#include "mainwindow.h"

#include <qsqlquery.h>
#include <qsqlerror.h>
#include <qsettings.h>
#include <qevent.h>
#include <qsettings.h>
#include "sortfilterproxymodel.h"

BooksListBrowser::BooksListBrowser(LibraryManager *libraryManager, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BooksListBrowser)
{
    ui->setupUi(this);

    loadSettings();
    m_libraryManager = libraryManager;

    m_filterModel = new SortFilterProxyModel(this);
    connect(ui->lineSearch, SIGNAL(textChanged(QString)), m_filterModel, SLOT(setFilterRegExp(QString)));// TODO: serach in book info...
    connect(ui->lineSearch, SIGNAL(textChanged(QString)), ui->treeView, SLOT(expandAll()));
    connect(m_libraryManager, SIGNAL(booksListModelLoaded(BooksListModel*)), SLOT(setModel(BooksListModel*)));
}

BooksListBrowser::~BooksListBrowser()
{
    delete ui;
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

void BooksListBrowser::setModel(BooksListModel *model)
{
    m_model = model;
    m_filterModel->setSourceModel(m_model);
    m_filterModel->setFilterKeyColumn(0);

    ui->treeView->setModel(m_filterModel);

//    ui->treeView->expandAll();
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
    if(node->type == BooksListNode::Book) {
        emit bookSelected(node->id);
    }
}
