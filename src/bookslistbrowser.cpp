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
#include "bookinfodialog.h"
#include "librarybookmanager.h"
#include "bookeditorview.h"

#include <qevent.h>
#include <qmenu.h>
#include <qmessagebox.h>

enum {
    OrderSort,
    BookTitleSort,
    AuthorNameSort,
    AuthorDeathSort
};

BooksListBrowser::BooksListBrowser(QWidget *parent) :
    QDialog(parent),
    m_bookListManager(LibraryManager::instance()->bookListManager()),
    m_favouritesManager(LibraryManager::instance()->favouritesManager()),
    m_bookManager(LibraryManager::instance()->bookManager()),
    m_currentModel(0),
    m_bookListModel(0),
    m_favouritesModel(0),
    m_lastReadedModel(0),
    m_bookListFilter(new ModelViewFilter(this)),
    ui(new Ui::BooksListBrowser)
{
    ui->setupUi(this);

    loadSettings();

    readBookListModel();
    readFavouritesModel();
    lastReadBooksModel();

    ui->checkSortAsc->setChecked(Utils::Settings::get("BooksListWidget/sortAsc", true).toBool());

    connect(m_bookListManager, SIGNAL(ModelsReady()), SLOT(readBookListModel()));
    connect(m_favouritesManager, SIGNAL(ModelsReady()), SLOT(readFavouritesModel()));
    connect(ui->treeBookList, SIGNAL(doubleClicked(QModelIndex)), SLOT(itemClicked(QModelIndex)));
    connect(ui->comboCurrentList, SIGNAL(currentIndexChanged(int)), SLOT(currentListChanged(int)));
    connect(ui->comboListSorting, SIGNAL(currentIndexChanged(int)), SLOT(listSortingChanged(int)));
    connect(ui->checkSortAsc, SIGNAL(toggled(bool)), SLOT(sortOrderChanged(bool)));

    connect(ui->treeBookList, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(bookListMenu(QPoint)));

    int current = Utils::Settings::get("BooksListWidget/currentList", 0).toInt();
    int sorting = Utils::Settings::get("BooksListWidget/listSorting", 0).toInt();

    if(current == ui->comboCurrentList->currentIndex())
        currentListChanged(current);
    else
        ui->comboCurrentList->setCurrentIndex(current);

    if(sorting == ui->comboListSorting->currentIndex())
        listSortingChanged(sorting);
    else
        ui->comboListSorting->setCurrentIndex(sorting);
}

BooksListBrowser::~BooksListBrowser()
{
    ml_delete_check(m_bookListModel);
    ml_delete_check(m_favouritesModel);
    ml_delete_check(m_lastReadedModel);

    delete ui;
}

void BooksListBrowser::setCurrentModel(TreeViewModel model)
{
    ui->comboCurrentList->setCurrentIndex(model);
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

    Utils::Widget::save(ui->treeBookList, "BooksListBrowser.bookList", 1);
}

void BooksListBrowser::setupListFilter()
{
    m_bookListFilter->reset();

    m_bookListFilter->setLineEdit(ui->lineFilterBookList);
    m_bookListFilter->setTreeView(ui->treeBookList);
    m_bookListFilter->setSourceModel(m_currentModel);

    m_bookListFilter->addFilterColumn(0, Qt::DisplayRole, tr("اسم الكتاب"));
    m_bookListFilter->addFilterColumn(1, Qt::DisplayRole, tr("المؤلف"));

    m_bookListFilter->setAllowFilterByDeath(true);

    m_bookListFilter->setup();

    Utils::Widget::restore(ui->treeBookList,
                           "BooksListBrowser.bookList",
                           QList<int>() << 350);
}

void BooksListBrowser::currentListChanged(int index)
{
    if(index == AllBooksModel) {
        m_currentModel = m_bookListModel;
    } else if(index == FavoritesModel) {
        m_currentModel = m_favouritesModel;
    } else if(index == LastOpenModel) {
        m_currentModel = 0; // don't call setupListFilter()
        lastReadBooksModel();
        m_currentModel = m_lastReadedModel;
    } else {
        qWarning() << "BooksListBrowser::currentListChanged unknow list index"
                   << index;
    }

    setupListFilter();

    Utils::Settings::set("BooksListWidget/currentList", index);
}

void BooksListBrowser::listSortingChanged(int index)
{
    int column = 0;
    int role = Qt::DisplayRole;
    Qt::SortOrder sort = (ui->checkSortAsc->isChecked()
                          ? Qt::AscendingOrder : Qt::DescendingOrder);

    if(index == OrderSort) {
        column = -1;
    } else if(index == BookTitleSort) {
        column = 0;
    } else if(index == AuthorNameSort) {
        column = 1;
    } else if(index == AuthorDeathSort) {
        column = 1;
        role = ItemRole::authorDeathRole;
    } else {
        qWarning() << "BooksListBrowser::listSortingChanged unknow list index"
                   << index;
    }

    m_bookListFilter->setSorting(column, role, sort);

    Utils::Settings::set("BooksListWidget/listSorting", index);
    Utils::Settings::set("BooksListWidget/sortAsc", ui->checkSortAsc->isChecked());
}

void BooksListBrowser::sortOrderChanged(bool checked)
{
    Q_UNUSED(checked);
    listSortingChanged(ui->comboListSorting->currentIndex());
}

void BooksListBrowser::readBookListModel()
{
    bool reload = (m_currentModel && m_currentModel == m_bookListModel);
    ml_delete_check(m_bookListModel);

    m_bookListModel = Utils::Model::cloneModel(m_bookListManager->bookListModel());
    ml_return_on_fail2(m_bookListModel, "BooksListBrowser::readBookListModel model is null");

    if(reload)
        setupListFilter();
}

void BooksListBrowser::readFavouritesModel()
{
    bool reload = (m_currentModel && m_currentModel == m_bookListModel);
    ml_delete_check(m_favouritesModel);

    m_favouritesModel = Utils::Model::cloneModel(m_favouritesManager->bookListModel());
    ml_return_on_fail2(m_favouritesModel, "BooksListBrowser::readFavouritesModel model is null");

    if(reload)
        setupListFilter();
}

void BooksListBrowser::lastReadBooksModel()
{
    bool reload = (m_currentModel && m_currentModel == m_bookListModel);
    ml_delete_check(m_lastReadedModel);

    m_lastReadedModel = Utils::Model::cloneModel(m_bookManager->getLastOpendModel().data());
    ml_return_on_fail2(m_lastReadedModel, "BooksListBrowser::lastReadBooksModel model is null");

    if(reload)
        setupListFilter();
}

void BooksListBrowser::itemClicked(QModelIndex index)
{
    int bookType = index.sibling(index.row(), 0).data(ItemRole::itemTypeRole).toInt();
    int bookID = index.sibling(index.row(), 0).data(ItemRole::idRole).toInt();
    int pageID = index.sibling(index.row(), 0).data(ItemRole::pageIdRole).toInt();

    ml_return_on_fail(bookType != ItemType::CategorieItem);

    if(m_currentModel != m_lastReadedModel)
        emit bookSelected(bookID);
    else
        emit bookSelected(bookID, pageID);
}

void BooksListBrowser::bookListMenu(QPoint /*point*/)
{
    QTreeView *treeView = qobject_cast<QTreeView*>(sender());
    ml_return_on_fail(treeView);

    QModelIndex index = Utils::Model::selectedIndex(treeView);
    ml_return_on_fail(index.isValid());

    int bookType = index.sibling(index.row(), 0).data(ItemRole::itemTypeRole).toInt();
    int bookID = index.sibling(index.row(), 0).data(ItemRole::idRole).toInt();

    ml_return_on_fail(bookType != ItemType::CategorieItem);

    QMenu menu(this);

    QAction *addToFavouriteAct = 0;
    QAction *removeFromFavouriteAct = 0;
    QAction *removeFromLastOpenedAct = 0;

    QAction *bookInfoAct = menu.addAction(QIcon(":/images/about.png"),
                                       tr("بطاقة الكتاب"));
    QAction *editBookAct = menu.addAction(QIcon::fromTheme("document-edit", QIcon(":/images/document-edit.png")),
                                          tr("تحرير الكتاب"));
    menu.addSeparator();

    if(!m_favouritesManager->containsBook(bookID)) {
        addToFavouriteAct = new QAction(QIcon::fromTheme("bookmark-new", QIcon(":/images/bookmark-new.png")),
                                        tr("اضافة الى المفضلة"),
                                        &menu);

        menu.addAction(addToFavouriteAct);
    } else {
        removeFromFavouriteAct = new QAction(tr("حذف من المفضلة"),
                                             &menu);
        menu.addAction(removeFromFavouriteAct);
    }

    QAction *searchInBookAct = menu.addAction(QIcon::fromTheme("edit-find", QIcon(":/images/find.png")),
                                           tr("بحث في الكتاب"));

    if(m_currentModel == m_lastReadedModel) {
        menu.addSeparator();

        removeFromLastOpenedAct = menu.addAction(QIcon(":/images/remove.png"),
                                                 tr("حذف من قائمة أحدث الكتب تصفحا"));
    }

    QAction *ret = menu.exec(QCursor::pos());
    if(ret) {
        if(ret == addToFavouriteAct) {
            m_favouritesManager->addBook(bookID, 0);
            m_favouritesManager->reloadModels();
        } else if(ret == removeFromFavouriteAct) {
            m_favouritesManager->removeBook(bookID);
            m_favouritesManager->reloadModels();
        } else if (ret == searchInBookAct) {
            MW->searchView()->newTab(SearchWidget::BookSearch, bookID);
            MW->showSearchView();
        } else if (ret == bookInfoAct) {
            LibraryBook::Ptr book = LibraryManager::instance()->bookManager()->getLibraryBook(bookID);
            ml_return_on_fail(book);

            BookInfoDialog *dialog = new BookInfoDialog(0);
            dialog->setLibraryBook(book);
            dialog->setup();
            dialog->show();
        } else if(ret == editBookAct) {
            LibraryBook::Ptr book = LibraryManager::instance()->bookManager()->getLibraryBook(bookID);
            ml_return_on_fail(book);

            MW->editorView()->editBook(book);
        } else if(ret == removeFromLastOpenedAct) {
            if(m_bookManager->deleteBookFromLastOpen(bookID)) {
                lastReadBooksModel();
            } else {
                QMessageBox::warning(this,
                                     tr("أحدث الكتب تصفحا"),
                                     tr("حدث خطأ أثناء حذف الكتاب من القائمة"));
            }
        }
    }
}
