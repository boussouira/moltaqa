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
    ml_delete_check(m_bookListModel);
    ml_delete_check(m_favouritesModel);

    delete ui;
}

void BooksListBrowser::closeEvent(QCloseEvent *event)
{
    saveSettings();

    event->accept();
}

void BooksListBrowser::loadSettings()
{
    Utils::Widget::restore(this, "BooksListWidget");
}

void BooksListBrowser::saveSettings()
{
    Utils::Widget::save(this, "BooksListWidget");

    Utils::Widget::save(ui->treeBookList, "BooksListBrowser.bookList", 2);
    Utils::Widget::save(ui->treeFavouritesList, "BooksListBrowser.favourites", 2);
}

void BooksListBrowser::readBookListModel()
{
    ml_delete_check(m_bookListModel);

    m_bookListModel = Utils::Model::cloneModel(m_bookListManager->bookListModel());
    ml_return_on_fail2(m_bookListModel, "BooksListBrowser::readBookListModel model is null");

    m_bookListFilter->setLineEdit(ui->lineFilterBookList);
    m_bookListFilter->setTreeView(ui->treeBookList);
    m_bookListFilter->setSourceModel(m_bookListModel);

    m_bookListFilter->setDefautSortRole(ItemRole::orderRole);
    m_bookListFilter->setDefautSortColumn(0, Qt::AscendingOrder);
    m_bookListFilter->setColumnSortRole(AuthorDeathCol, ItemRole::authorDeathRole);
    m_bookListFilter->setup();

    Utils::Widget::restore(ui->treeBookList,
                           "BooksListBrowser.bookList",
                           QList<int>() << 350 << 200);
}

void BooksListBrowser::readFavouritesModel()
{
    ml_delete_check(m_favouritesModel);

    m_favouritesModel = Utils::Model::cloneModel(m_favouritesManager->bookListModel());
    ml_return_on_fail2(m_favouritesModel, "BooksListBrowser::readFavouritesModel model is null");

    m_favouritesListFilter->setLineEdit(ui->lineFilterFavourites);
    m_favouritesListFilter->setTreeView(ui->treeFavouritesList);
    m_favouritesListFilter->setSourceModel(m_favouritesModel);

    m_favouritesListFilter->setDefautSortRole(ItemRole::orderRole);
    m_favouritesListFilter->setDefautSortColumn(0, Qt::AscendingOrder);
    m_favouritesListFilter->setColumnSortRole(AuthorDeathCol, ItemRole::authorDeathRole);
    m_favouritesListFilter->setup();

    Utils::Widget::restore(ui->treeFavouritesList,
                           "BooksListBrowser.favourites",
                           QList<int>() << 350 << 200);
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
    QModelIndex index = Utils::Model::selectedIndex(ui->treeBookList);
    ml_return_on_fail(index.isValid());

    int bookType = index.sibling(index.row(), 0).data(ItemRole::itemTypeRole).toInt();
    int bookID = index.sibling(index.row(), 0).data(ItemRole::idRole).toInt();

    ml_return_on_fail(bookType != ItemType::CategorieItem);

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
