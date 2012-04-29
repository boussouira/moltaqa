#include "welcomewidget.h"
#include "ui_welcomewidget.h"
#include "librarymanager.h"
#include "modelenums.h"
#include "modelutils.h"
#include "mainwindow.h"
#include "booklistmanager.h"
#include "favouritesmanager.h"
#include "modelviewfilter.h"
#include "librarybookmanager.h"
#include "utils.h"

#include <qsettings.h>
#include <qmenu.h>

enum {
    BookNameCol,
    AuthorNameCol,
    AuthorDeathCol,
    ColumnCount
};

WelcomeWidget::WelcomeWidget(QWidget *parent) :
    AbstarctView(parent),
    m_bookListManager(LibraryManager::instance()->bookListManager()),
    m_favouritesManager(LibraryManager::instance()->favouritesManager()),
    m_bookManager(LibraryManager::instance()->bookManager()),
    m_bookListModel(0),
    m_favouritesModel(0),
    m_lastReadedModel(0),
    m_bookListFilter(new ModelViewFilter(this)),
    m_favouritesListFilter(new ModelViewFilter(this)),
    ui(new Ui::WelcomeWidget)
{
    ui->setupUi(this);

    bookListModel();
    favouritesModel();

    connect(m_bookListManager, SIGNAL(ModelsReady()),
            SLOT(bookListModel()));
    connect(m_favouritesManager, SIGNAL(ModelsReady()),
            SLOT(favouritesModel()));
    connect(ui->treeBookList, SIGNAL(doubleClicked(QModelIndex)),
            SLOT(itemClicked(QModelIndex)));
    connect(ui->treeFavouritesList, SIGNAL(doubleClicked(QModelIndex)),
            SLOT(itemClicked(QModelIndex)));
    connect(ui->treeLastBook, SIGNAL(doubleClicked(QModelIndex)),
            SLOT(lastOpenedItemClicked(QModelIndex)));
}

WelcomeWidget::~WelcomeWidget()
{
    saveSettings();

    ml_delete_check(m_bookListModel);
    ml_delete_check(m_favouritesModel);
    ml_delete_check(m_lastReadedModel);

    delete ui;
}

QString WelcomeWidget::title()
{
    return tr("صفحة البداية");
}

void WelcomeWidget::aboutToShow()
{
    loadSettings();
    lastReadBooksModel();
}

void WelcomeWidget::aboutToHide()
{
    QSettings settings;
    settings.beginGroup("WelcomeWidget");

    settings.setValue("tab", ui->tabWidget->currentIndex());
}

void WelcomeWidget::loadSettings()
{
    QSettings settings;
    settings.beginGroup("WelcomeWidget");

    ui->tabWidget->setCurrentIndex(settings.value("tab", 0).toInt());
}

void WelcomeWidget::saveSettings()
{
    QSettings settings;
    settings.beginGroup("WelcomeWidget");
    settings.setValue("tab", ui->tabWidget->currentIndex());

    if(m_bookListModel)
        Utils::Widget::save(ui->treeBookList, "WelcomeWidget.bookList", 2);

    if(m_favouritesModel)
        Utils::Widget::save(ui->treeFavouritesList, "WelcomeWidget.favourites", 2);

    if(m_lastReadedModel)
        Utils::Widget::save(ui->treeLastBook, "WelcomeWidget.lastBook", 1);
}

void WelcomeWidget::bookListModel()
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
                           "WelcomeWidget.bookList",
                           QList<int>() << 350 << 200);
}

void WelcomeWidget::favouritesModel()
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
                           "WelcomeWidget.favourites",
                           QList<int>() << 350 << 200);
}

void WelcomeWidget::lastReadBooksModel()
{
    ml_delete_check(m_lastReadedModel);

    m_lastReadedModel = Utils::Model::cloneModel(m_bookManager->getLastOpendModel().data());
    ml_return_on_fail2(m_lastReadedModel, "WelcomeWidget::lastReadBooksModel model is null");

    ui->treeLastBook->setModel(m_lastReadedModel);

    Utils::Widget::restore(ui->treeLastBook,
                           "WelcomeWidget.lastBook",
                           QList<int>() << 350);
}

void WelcomeWidget::itemClicked(QModelIndex index)
{
    int bookType = index.sibling(index.row(), 0).data(ItemRole::itemTypeRole).toInt();
    int bookID = index.sibling(index.row(), 0).data(ItemRole::idRole).toInt();

    if(bookType != ItemType::CategorieItem)
        emit bookSelected(bookID);
}

void WelcomeWidget::lastOpenedItemClicked(QModelIndex index)
{
    int book = index.sibling(index.row(), 0).data(ItemRole::bookIdRole).toInt();
    int page = index.sibling(index.row(), 0).data(ItemRole::idRole).toInt();

    MW->openBook(book, page);
}
