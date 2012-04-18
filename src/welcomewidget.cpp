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

    ui->webView->setUrl(QUrl("http://www.ahlalhdeeth.com/vb/forumdisplay.php?f=75"));

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

    ML_DELETE_CHECK(m_bookListModel);
    ML_DELETE_CHECK(m_favouritesModel);
    ML_DELETE_CHECK(m_lastReadedModel);

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
}

void WelcomeWidget::loadSettings()
{
    QSettings settings;
    settings.beginGroup("WelcomeWidget");

    if(settings.contains("splitter"))
        ui->splitter->restoreState(settings.value("splitter").toByteArray());

    ui->tabWidget->setCurrentIndex(settings.value("tab", 0).toInt());
}

void WelcomeWidget::saveSettings()
{
    QSettings settings;
    settings.beginGroup("WelcomeWidget");

    settings.setValue("splitter", ui->splitter->saveState());
    settings.setValue("tab", ui->tabWidget->currentIndex());

    if(m_bookListModel) {
        settings.setValue("bcol_1", ui->treeBookList->columnWidth(0));
        settings.setValue("bcol_2", ui->treeBookList->columnWidth(1));
    }

    if(m_favouritesModel) {
        settings.setValue("fcol_1", ui->treeFavouritesList->columnWidth(0));
        settings.setValue("fcol_2", ui->treeFavouritesList->columnWidth(1));
    }
}

void WelcomeWidget::bookListModel()
{
    ML_DELETE_CHECK(m_bookListModel);

    m_bookListModel = Utils::Model::cloneModel(m_bookListManager->bookListModel());
    ML_ASSERT2(m_bookListModel, "BooksListBrowser::readBookListModel model is null");

    m_bookListFilter->setLineEdit(ui->lineFilterBookList);
    m_bookListFilter->setTreeView(ui->treeBookList);
    m_bookListFilter->setSourceModel(m_bookListModel);

    m_bookListFilter->setDefautSortRole(ItemRole::orderRole);
    m_bookListFilter->setDefautSortColumn(0, Qt::AscendingOrder);
    m_bookListFilter->setColumnSortRole(AuthorDeathCol, ItemRole::authorDeathRole);
    m_bookListFilter->setup();

    QSettings settings;
    settings.beginGroup("WelcomeWidget");

    ui->treeBookList->setColumnWidth(BookNameCol,
                                 settings.value("bcol_1", 350).toInt());

    ui->treeBookList->setColumnWidth(AuthorNameCol,
                                 settings.value("bcol_2", 200).toInt());
}

void WelcomeWidget::favouritesModel()
{
    ML_DELETE_CHECK(m_favouritesModel);

    m_favouritesModel = Utils::Model::cloneModel(m_favouritesManager->bookListModel());
    ML_ASSERT2(m_favouritesModel, "BooksListBrowser::readFavouritesModel model is null");

    m_favouritesListFilter->setLineEdit(ui->lineFilterFavourites);
    m_favouritesListFilter->setTreeView(ui->treeFavouritesList);
    m_favouritesListFilter->setSourceModel(m_favouritesModel);

    m_favouritesListFilter->setDefautSortRole(ItemRole::orderRole);
    m_favouritesListFilter->setDefautSortColumn(0, Qt::AscendingOrder);
    m_favouritesListFilter->setColumnSortRole(AuthorDeathCol, ItemRole::authorDeathRole);
    m_favouritesListFilter->setup();

    QSettings settings;
    settings.beginGroup("WelcomeWidget");

    ui->treeFavouritesList->setColumnWidth(BookNameCol,
                                 settings.value("fcol_1", 350).toInt());

    ui->treeFavouritesList->setColumnWidth(AuthorNameCol,
                                           settings.value("fcol_2", 200).toInt());
}

void WelcomeWidget::lastReadBooksModel()
{
    ML_DELETE_CHECK(m_lastReadedModel);

    m_lastReadedModel = Utils::Model::cloneModel(m_bookManager->getLastOpendModel().data());
    ML_ASSERT2(m_lastReadedModel, "WelcomeWidget::lastReadBooksModel model is null");

    ui->treeLastBook->setModel(m_lastReadedModel);
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
