#include "bookslistbrowser.h"
#include "ui_bookslistbrowser.h"
#include "librarymanager.h"
#include "modelenums.h"
#include "mainwindow.h"
#include "booklistmanager.h"
#include "utils.h"

#include <qsqlquery.h>
#include <qsqlerror.h>
#include <qsettings.h>
#include <qevent.h>
#include <qsettings.h>
#include "sortfilterproxymodel.h"

enum {
    BookNameCol,
    AuthorNameCol,
    AuthorDeathCol,
    ColumnCount
};

BooksListBrowser::BooksListBrowser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BooksListBrowser)
{
    ui->setupUi(this);

    loadSettings();
    m_bookListManager = LibraryManager::instance()->bookListManager();

    m_model = 0;
    m_filterModel = new SortFilterProxyModel(this);

    setModel(m_bookListManager->bookListModel());

    connect(ui->lineSearch, SIGNAL(textChanged(QString)),
            SLOT(setFilterText(QString)));// TODO: serach in book info...

    connect(m_bookListManager, SIGNAL(ModelsReady()), SLOT(readBookListModel()));

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
    saveSettings();

    event->accept();
}

void BooksListBrowser::loadSettings()
{
    Utils::restoreWidgetPosition(this, "BooksListWidget");
}

void BooksListBrowser::saveSettings()
{
    Utils::saveWidgetPosition(this, "BooksListWidget");

    if(m_model) {
        QSettings settings;
        settings.setValue("BooksListWidget/col_1", ui->treeView->columnWidth(0));
        settings.setValue("BooksListWidget/col_2", ui->treeView->columnWidth(1));
    }
}

void BooksListBrowser::readBookListModel()
{
    setModel(m_bookListManager->bookListModel());
}

void BooksListBrowser::setModel(QStandardItemModel *model)
{
    QSettings settings;
    settings.beginGroup("BooksListWidget");

    m_model = model;
    m_filterModel->setSourceModel(m_model);
    m_filterModel->setFilterKeyColumn(BookNameCol);

    ui->treeView->setModel(m_filterModel);

    ui->treeView->sortByColumn(0, Qt::AscendingOrder);
    m_filterModel->setSortRole(ItemRole::orderRole);

    ui->treeView->setColumnWidth(BookNameCol,
                                 settings.value("col_1", 350).toInt());

    ui->treeView->setColumnWidth(AuthorNameCol,
                                 settings.value("col_2", 200).toInt());
}

void BooksListBrowser::setFilterText(QString text)
{
    if(text.size() > 2) {
        text.replace(QRegExp("[\\x0627\\x0622\\x0623\\x0625]"), "[\\x0627\\x0622\\x0623\\x0625]");//ALEFs
        text.replace(QRegExp("[\\x0647\\x0629]"), "[\\x0647\\x0629]"); //TAH_MARBUTA, HEH
        text.replace(QRegExp("[\\x064A\\x0649]"), "[\\x064A\\x0649]"); //YAH, ALEF MAKSOURA

        m_filterModel->setFilterRegExp(text);
        ui->treeView->expandAll();
    } else {
        m_filterModel->setFilterFixedString("");
        ui->treeView->collapseAll();
    }
}

void BooksListBrowser::sortChanged(int logicalIndex, Qt::SortOrder)
{

    if(logicalIndex != AuthorDeathCol)
        m_filterModel->setSortRole(Qt::DisplayRole);
    else
        m_filterModel->setSortRole(ItemRole::authorDeathRole);
}

void BooksListBrowser::on_treeView_doubleClicked(QModelIndex index)
{
    int bookType = index.sibling(index.row(), 0).data(ItemRole::itemTypeRole).toInt();
    int bookID = index.sibling(index.row(), 0).data(ItemRole::idRole).toInt();

    if(bookType != ItemType::CategorieItem)
        emit bookSelected(bookID);
}
