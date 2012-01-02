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

    m_model = 0;
    m_filterModel = new SortFilterProxyModel(this);

    connect(ui->lineSearch, SIGNAL(textChanged(QString)),
            SLOT(setFilterText(QString)));// TODO: serach in book info...

    connect(m_libraryManager, SIGNAL(booksListModelLoaded(BooksListModel*)),
            SLOT(setModel(BooksListModel*)));

    connect(ui->treeView->header(),
            SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),
            SLOT(sortChanged(int,Qt::SortOrder)));
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

    if(m_model) {
        settings.setValue("col_1", ui->treeView->columnWidth(0));
        settings.setValue("col_2", ui->treeView->columnWidth(1));
    }

    settings.endGroup();

    event->accept();
}

void BooksListBrowser::loadSettings()
{
    QSettings settings;
    settings.beginGroup("BooksListWidget");
    move(settings.value("pos", pos()).toPoint());
    resize(settings.value("size", QSize(680, 500)).toSize());
    settings.endGroup();
}

void BooksListBrowser::setModel(BooksListModel *model)
{
    QSettings settings;
    settings.beginGroup("BooksListWidget");

    m_model = model;
    m_filterModel->setSourceModel(m_model);
    m_filterModel->setFilterKeyColumn(BooksListModel::BookNameCol);

    ui->treeView->setModel(m_filterModel);

    ui->treeView->sortByColumn(0, Qt::AscendingOrder);
    m_filterModel->setSortRole(ItemRole::orderRole);

    ui->treeView->setColumnWidth(BooksListModel::BookNameCol,
                                 settings.value("col_1", 350).toInt());

    ui->treeView->setColumnWidth(BooksListModel::AuthorNameCol,
                                 settings.value("col_2", 200).toInt());
}

void BooksListBrowser::setFilterText(QString text)
{
    if(text.size() > 2) {
        m_filterModel->setFilterRegExp(text);
        ui->treeView->expandAll();
    } else {
        m_filterModel->setFilterFixedString("");
        ui->treeView->collapseAll();
    }
}

void BooksListBrowser::sortChanged(int logicalIndex, Qt::SortOrder)
{

    if(logicalIndex != BooksListModel::AuthorDeathCol)
        m_filterModel->setSortRole(Qt::DisplayRole);
    else
        m_filterModel->setSortRole(ItemRole::authorDeathRole);
}

void BooksListBrowser::on_treeView_doubleClicked(QModelIndex index)
{
    BooksListModel *model = static_cast<BooksListModel*>(ui->treeView->model());
    BooksListNode *node = model->nodeFromIndex(m_filterModel->mapToSource(index));
    if(node->type == BooksListNode::Book) {
        emit bookSelected(node->id);
    }
}
