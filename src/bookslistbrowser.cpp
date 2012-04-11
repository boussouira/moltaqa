#include "bookslistbrowser.h"
#include "ui_bookslistbrowser.h"
#include "librarymanager.h"
#include "modelenums.h"
#include "modelutils.h"
#include "mainwindow.h"
#include "booklistmanager.h"
#include "favouritesmanager.h"
#include "modelviewfilter.h"
#include "utils.h"

#include <qevent.h>
#include <qsettings.h>
#include <qmenu.h>

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
    m_favouritesManager = LibraryManager::instance()->favouritesManager();

    m_bookListModel = 0;
    m_favouritesModel = 0;

    m_bookListFilter = new ModelViewFilter(this);
    m_favouritesListFilter = new ModelViewFilter(this);

    readBookListModel();
    readFavouritesModel();

    connect(m_bookListManager, SIGNAL(ModelsReady()), SLOT(readBookListModel()));
    connect(m_favouritesManager, SIGNAL(ModelsReady()), SLOT(readFavouritesModel()));

    connect(ui->treeBookList, SIGNAL(doubleClicked(QModelIndex)),
            SLOT(itemClicked(QModelIndex)));
    connect(ui->treeFavouritesList, SIGNAL(doubleClicked(QModelIndex)),
            SLOT(itemClicked(QModelIndex)));

    connect(ui->treeBookList, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(bookListMenu(QPoint)));
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

    QSettings settings;

    if(m_bookListModel) {
        settings.setValue("BooksListWidget/bcol_1", ui->treeBookList->columnWidth(0));
        settings.setValue("BooksListWidget/bcol_2", ui->treeBookList->columnWidth(1));
    }

    if(m_favouritesModel) {
        settings.setValue("BooksListWidget/fcol_1", ui->treeFavouritesList->columnWidth(0));
        settings.setValue("BooksListWidget/fcol_2", ui->treeFavouritesList->columnWidth(1));
    }
}

void BooksListBrowser::readBookListModel()
{
    m_bookListModel = m_bookListManager->bookListModel();
    ML_ASSERT2(m_bookListModel, "BooksListBrowser::readBookListModel model is null");

    m_bookListFilter->setLineEdit(ui->lineFilterBookList);
    m_bookListFilter->setTreeView(ui->treeBookList);
    m_bookListFilter->setSourceModel(m_bookListModel);

    m_bookListFilter->setDefautSortRole(ItemRole::orderRole);
    m_bookListFilter->setDefautSortColumn(0, Qt::AscendingOrder);
    m_bookListFilter->setColumnSortRole(AuthorDeathCol, ItemRole::authorDeathRole);
    m_bookListFilter->setup();

    QSettings settings;
    settings.beginGroup("BooksListWidget");

    ui->treeBookList->setColumnWidth(BookNameCol,
                                 settings.value("bcol_1", 350).toInt());

    ui->treeBookList->setColumnWidth(AuthorNameCol,
                                 settings.value("bcol_2", 200).toInt());
}

void BooksListBrowser::readFavouritesModel()
{
    m_favouritesModel = m_favouritesManager->bookListModel();
    ML_ASSERT2(m_favouritesModel, "BooksListBrowser::readFavouritesModel model is null");

    m_favouritesListFilter->setLineEdit(ui->lineFilterFavourites);
    m_favouritesListFilter->setTreeView(ui->treeFavouritesList);
    m_favouritesListFilter->setSourceModel(m_favouritesModel);

    m_favouritesListFilter->setDefautSortRole(ItemRole::orderRole);
    m_favouritesListFilter->setDefautSortColumn(0, Qt::AscendingOrder);
    m_favouritesListFilter->setColumnSortRole(AuthorDeathCol, ItemRole::authorDeathRole);
    m_favouritesListFilter->setup();

    QSettings settings;
    settings.beginGroup("BooksListWidget");

    ui->treeFavouritesList->setColumnWidth(BookNameCol,
                                 settings.value("fcol_1", 350).toInt());

    ui->treeFavouritesList->setColumnWidth(AuthorNameCol,
                                 settings.value("fcol_2", 200).toInt());
}

void BooksListBrowser::itemClicked(QModelIndex index)
{
    int bookType = index.sibling(index.row(), 0).data(ItemRole::itemTypeRole).toInt();
    int bookID = index.sibling(index.row(), 0).data(ItemRole::idRole).toInt();

    if(bookType != ItemType::CategorieItem)
        emit bookSelected(bookID);
}

void BooksListBrowser::bookListMenu(QPoint /*point*/)
{
    QModelIndex index = Utils::selectedIndex(ui->treeBookList);
    ML_ASSERT(index.isValid());

    int bookType = index.sibling(index.row(), 0).data(ItemRole::itemTypeRole).toInt();
    int bookID = index.sibling(index.row(), 0).data(ItemRole::idRole).toInt();

    ML_ASSERT(bookType != ItemType::CategorieItem);

    QMenu menu(this);

    QAction *addToFavouriteAct = 0;
    QAction *removeFromFavouriteAct = 0;

    if(!m_favouritesManager->containsBook(bookID)) {
        addToFavouriteAct = new QAction(QIcon::fromTheme("bookmark-new", QIcon(":/images/bookmark-new.png")),
                                        tr("اضافة الى المفضلة"),
                                        &menu);

        menu.addAction(addToFavouriteAct);
    } else {
        removeFromFavouriteAct = new QAction(QIcon(":/images/remove.png"),
                                             tr("حذف من المفضلة"),
                                             &menu);
        menu.addAction(removeFromFavouriteAct);
    }

    QAction *ret = menu.exec(QCursor::pos());
    if(ret) {
        if(ret == addToFavouriteAct) {
            m_favouritesManager->addBook(bookID, 0);
            m_favouritesManager->reloadModels();
        } else if(ret == removeFromFavouriteAct) {
            m_favouritesManager->removeBook(bookID);
            m_favouritesManager->reloadModels();
        }
    }
}
