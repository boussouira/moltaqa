#include "booklistmanagerwidget.h"
#include "ui_booklistmanagerwidget.h"
#include "mainwindow.h"
#include "selectcatdialog.h"
#include "selectbooksdialog.h"
#include "booklistmanager.h"
#include "authorsmanager.h"
#include "modelutils.h"
#include "modelenums.h"
#include "utils.h"
#include "timeutils.h"
#include "librarymanager.h"
#include "modelviewsearcher.h"

#include <qabstractitemmodel.h>
#include <qmessagebox.h>
#include <qdebug.h>
#include <qmenu.h>
#include <qaction.h>
#include <qinputdialog.h>
#include <qboxlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qprogressdialog.h>

BookListManagerWidget::BookListManagerWidget(QWidget *parent) :
    ControlCenterWidget(parent),
    m_model(0),
    ui(new Ui::BookListManagerWidget)
{
    ui->setupUi(this);

    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    m_manager = LibraryManager::instance()->bookListManager();
    m_treeManager = new TreeViewEditor(this);
    m_viewSearcher = new ModelViewSearcher(this);
    m_updateBooksModel = 0;

    m_treeManager->setMoveUpButton(ui->toolMoveUp);
    m_treeManager->setMoveDownButton(ui->toolMoveDown);
    m_treeManager->setMoveLeftButton(ui->toolMoveLeft);
    m_treeManager->setMoveRightButton(ui->toolMoveRight);
    m_treeManager->setRemovButton(ui->toolRemoveCat);

    connect(ui->toolAddCat, SIGNAL(clicked()), SLOT(addToBookList()));
    connect(ui->toolUpdateList, SIGNAL(clicked()), SLOT(updateBooksList()));
    connect(m_manager, SIGNAL(ModelsReady()), SLOT(reloadModel()));

    connect(ui->treeView, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(menuRequested(QPoint)));
}

BookListManagerWidget::~BookListManagerWidget()
{
    ml_delete_check(m_model);
    ml_delete_check(m_treeManager);

    delete ui;
}

QString BookListManagerWidget::title()
{
    return tr("الأقسام");
}

void BookListManagerWidget::aboutToShow()
{
    if(m_manager->domHelper().needSave()) {
        m_manager->reloadModels();
        reloadModel();
    }
}

void BookListManagerWidget::loadModel()
{
    ml_delete_check(m_model);

    m_model = Utils::Model::cloneModel(m_manager->bookListModel());

    ui->treeView->setModel(m_model);
    ui->treeView->resizeColumnToContents(0);

    m_treeManager->setTreeView(ui->treeView);
    m_treeManager->setModel(m_model);
    m_treeManager->setup();

    m_viewSearcher->setTreeView(ui->treeView);
    m_viewSearcher->setSourceModel(m_model);
    m_viewSearcher->setLineEdit(ui->filterLineEdit);
    m_viewSearcher->setup();
}

void BookListManagerWidget::save()
{
    if(m_treeManager->isDataChanged()) {
        m_manager->save(m_model);
        m_treeManager->setDataChanged(false);
    }
}

void BookListManagerWidget::reloadModel()
{
    loadModel();
}

void BookListManagerWidget::copyNode()
{
    QModelIndexList list = ui->treeView->selectionModel()->selectedRows();
    ml_return_on_fail(list.size());
    qSort(list);

    if(m_cattedItems.isEmpty()) {
        m_copiedItems.clear();

        for(int i=list.size()-1;i>=0;i--) {
            QModelIndex index = list[i];
            if(index.isValid()) {
                QStandardItem *item = Utils::Model::itemFromIndex(m_model, index);
                if(item)
                    m_copiedItems << Utils::Model::cloneItem(item,
                                                             Utils::Model::itemFromIndex(m_model, index.parent()),
                                                             m_model->columnCount());
            }
        }
    }
}

void BookListManagerWidget::cutNode()
{
    QModelIndexList list = ui->treeView->selectionModel()->selectedRows();
    ml_return_on_fail(list.size());
    qSort(list);

    if(m_cattedItems.isEmpty()) {
        for(int i=list.size()-1;i>=0;i--) {
            QModelIndex index = list[i];
            if(index.isValid()) {
                QStandardItem *parentItem = Utils::Model::itemFromIndex(m_model, index.parent());
                if(parentItem) {
                    m_cattedItems << parentItem->takeRow(index.row());

                    //ui->treeView->expand(index);
                }
            }
        }

        m_copiedItems.clear();
    }
}

void BookListManagerWidget::pastNode()
{
    QModelIndex index = Utils::Model::selectedIndex(ui->treeView);
    ml_return_on_fail(index.isValid());

    QList<QList<QStandardItem*> > items = m_copiedItems.size() ? m_copiedItems : m_cattedItems;

    if(items.size()) {
        for(int i=items.size()-1;i>=0;i--) {
            QList<QStandardItem*> row = items[i];
            QStandardItem *item = m_model->itemFromIndex(index);

            item->appendRow(row);

            Utils::Model::selectIndex(ui->treeView, index.child(item->rowCount()-1, 0));
        }

        m_copiedItems.clear();
        m_cattedItems.clear();
    }
}

void BookListManagerWidget::pastSublingNode()
{
    QModelIndex index = Utils::Model::selectedIndex(ui->treeView);
    ml_return_on_fail(index.isValid());

    QList<QList<QStandardItem*> > items = m_copiedItems.size() ? m_copiedItems : m_cattedItems;

    if(items.size()) {
        for(int i=items.size()-1;i>=0;i--) {
            QList<QStandardItem*> row = items[i];
            QStandardItem *parentItem = Utils::Model::itemFromIndex(m_model, index.parent());
            parentItem->insertRow(index.row()+1, row);

            Utils::Model::selectIndex(ui->treeView, index.sibling(index.row()+1, 0));
        }

        m_copiedItems.clear();
        m_cattedItems.clear();
    }
}

void BookListManagerWidget::addToBookList()
{
    QMenu menu(this);
    menu.addAction(tr("اضافة قسم"), this, SLOT(addCat()));
    menu.addAction(tr("اضافة كتب"), this, SLOT(addBooks()));
    menu.exec(QCursor::pos());
}

void BookListManagerWidget::addCat()
{
    QModelIndex index = Utils::Model::selectedIndex(ui->treeView);
    QStandardItem *parentItem = Utils::Model::itemFromIndex(m_model, index.parent());

    bool ok = false;
    QString title = QInputDialog::getText(this, tr("اسم القسم"),
                                         tr("ادخل اسم القسم:"), QLineEdit::Normal,
                                         QString(), &ok);
    if(ok && title.size()) {
        QStandardItem *catItem = new QStandardItem();
        catItem->setText(title);
        catItem->setIcon(QIcon(":/images/book-cat.png"));
        catItem->setData(m_manager->getNewCategorieID(),
                         ItemRole::idRole);
        catItem->setData(ItemType::CategorieItem, ItemRole::itemTypeRole);

        parentItem->insertRow(index.row()+1, catItem);

        Utils::Model::selectIndex(ui->treeView, index.sibling(index.row()+1, 0));
    }
}

void BookListManagerWidget::addBooks()
{
    selectBooksDialog dialog(this);
    if(dialog.exec() == QDialog::Accepted) {
        QList<int> books = dialog.selectedBooks();

        QModelIndex parent = Utils::Model::selectedIndex(ui->treeView);
        while(parent.isValid()) {
            if(parent.data(ItemRole::itemTypeRole).toInt() == ItemType::CategorieItem) {
                break;
            }

            parent = parent.parent();
        }

        foreach (int bookID, books) {
            LibraryBook::Ptr book = LibraryManager::instance()->bookManager()->getLibraryBook(bookID);
            if(!book)
                continue;

            addBookItem(book, parent);

            if(parent.isValid()) {
                QStandardItem *item = Utils::Model::itemFromIndex(m_model, parent);
                if(item)
                    Utils::Model::selectIndex(ui->treeView, parent.child(item->rowCount()-1, 0));
            } else {
                Utils::Model::selectIndex(ui->treeView, m_model->index(m_model->rowCount()-1, 0));
            }
        }
    }
}

void BookListManagerWidget::menuRequested(QPoint)
{
    QModelIndexList list = ui->treeView->selectionModel()->selectedRows();
    qSort(list);

    bool sameLevel = Utils::Model::indexesAtSameLevel(list);

    QMenu menu(this);
    QAction *copyAct = new QAction(tr("نسخ"), &menu);
    QAction *cutAct = new QAction(tr("قص"), &menu);
    QAction *pastAct = new QAction(tr("لصق"), &menu);
    QAction *pastSublingAct = new QAction(tr("لصق في نفس المستوى"), &menu);

    copyAct->setEnabled(m_cattedItems.isEmpty() && sameLevel);
    cutAct->setEnabled(m_cattedItems.isEmpty() && sameLevel);
    pastAct->setEnabled((m_copiedItems.size() || m_cattedItems.size()) && list.size()==1);
    pastSublingAct->setEnabled(pastAct->isEnabled());

    menu.addAction(copyAct);
    menu.addAction(cutAct);
    menu.addAction(pastAct);
    menu.addAction(pastSublingAct);

    QAction *ret = menu.exec(QCursor::pos());
    if(ret) {
        if(ret == cutAct)
            cutNode();
        else if(ret == copyAct)
            copyNode();
        else if(ret == pastAct)
            pastNode();
        else if(ret == pastSublingAct)
            pastSublingNode();
    }
}


void BookListManagerWidget::addBookItem(LibraryBook::Ptr book, const QModelIndex &parent)
{
    QList<QStandardItem*> rows;

    QStandardItem *nameItem = new QStandardItem();
    nameItem->setText(book->title);
    nameItem->setToolTip(book->comment);
    nameItem->setIcon(QIcon(":/images/book.png"));
    nameItem->setData(book->id, ItemRole::idRole);
    nameItem->setData(ItemType::BookItem, ItemRole::itemTypeRole);
    nameItem->setData(book->type, ItemRole::typeRole);

    rows << nameItem;

    if(book->type != LibraryBook::QuranBook) {
        QStandardItem *authItem = new QStandardItem();
        AuthorInfo::Ptr auth = LibraryManager::instance()->authorsManager()->getAuthorInfo(book->authorID);
        if(auth) {
            authItem->setText(LibraryManager::instance()->authorsManager()->formatAuthorName(auth));
            authItem->setData(book->authorID, ItemRole::authorIdRole);
            authItem->setData(auth->deathYear, ItemRole::authorDeathRole);
        }

        rows << authItem;
    }

    QStandardItem *item = Utils::Model::itemFromIndex(m_model, parent);
    item->appendRow(rows);
}

void BookListManagerWidget::modelEdited()
{
    setModified(true);
}

void BookListManagerWidget::updateBooksList()
{
    int rep = QMessageBox::question(this,
                                    tr("لائحة الكتب"),
                                    tr("هل تريد تحديث قائمة الكتب؟"),
                                    QMessageBox::Yes|QMessageBox::No, QMessageBox::No);

    ml_return_on_fail(rep == QMessageBox::Yes);

    ml_delete_check(m_updateBooksModel);
    m_updateBooksModel = new QStandardItemModel(this);

    QProgressDialog dialog(this);
    dialog.setWindowTitle(tr("تحديث اللائحة"));
    dialog.setLabelText(tr("جاري تحديث لائحة الكتب..."));
    dialog.setMaximum(m_model->rowCount());
    dialog.setCancelButton(0);
    dialog.show();

    QModelIndex index = m_model->index(0, 0);
    while(index.isValid()) {
        updateItem(index);
        dialog.setValue(dialog.value()+1);

        index = index.sibling(index.row()+1, index.column());
    }

    dialog.hide();

    if(m_updateBooksModel->rowCount() > 0) {
        QLabel *label = new QLabel(tr("التغييرات التي طرأت على لائحة الكتب:"));

        QTreeView *tree = new QTreeView();
        tree->setHeaderHidden(true);
        tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tree->setModel(m_updateBooksModel);

        QVBoxLayout *layout = new QVBoxLayout();
        layout->addWidget(label);
        layout->addWidget(tree);

        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle(tr("تحديث لائحة الكتب"));
        dialog->setLayout(layout);
        dialog->resize(420, 250);
        dialog->exec();
    } else {
        QMessageBox::information(this,
                                 tr("تحديث اللائحة"),
                                 tr("تم تحديث لائحة الكتب"));
    }
}

void BookListManagerWidget::updateItem(QModelIndex index)
{
    if(index.data(ItemRole::itemTypeRole).toInt() == ItemType::BookItem) {
        QModelIndex authorIndex = index.sibling(index.row(), 1);

        int bookID = index.data(ItemRole::idRole).toInt();
        int authorID = authorIndex.data(ItemRole::authorIdRole).toInt();
        bool isQuran = index.data(ItemRole::typeRole).toInt() == LibraryBook::QuranBook;

        QStandardItem *bookItem = new QStandardItem(tr("كتاب '%1'").arg(index.data().toString()));
        LibraryBook::Ptr book = LibraryManager::instance()->bookManager()->getLibraryBook(bookID);
        if(book) {
            bookItem->setIcon(QIcon(":/images/refresh2.png"));
            QList<QStandardItem*> bookItems;
            if(index.data().toString() != book->title) {
                bookItems << new QStandardItem(tr("تغيير عنوان الكتاب من '%1' الى '%2'")
                                               .arg(index.data().toString())
                                               .arg(book->title));
                m_model->setData(index, book->title, Qt::EditRole);

            }

            if(authorID != book->authorID) {
                m_model->setData(authorIndex, book->authorID, ItemRole::authorIdRole);
                authorID = book->authorID;
            }

            if(authorID && !isQuran) {
                AuthorInfo::Ptr author = LibraryManager::instance()->authorsManager()->getAuthorInfo(authorID);
                if(author) {
                    QString authorStr = LibraryManager::instance()->authorsManager()->formatAuthorName(author);
                    if(authorStr != authorIndex.data().toString()) {
                        bookItems << new QStandardItem(tr("تغيير اسم المؤلف من '%1' الى '%2'")
                                                       .arg(authorIndex.data().toString())
                                                       .arg(authorStr));
                        m_model->setData(authorIndex, authorStr, Qt::EditRole);
                    }

                    if(author->deathYear != authorIndex.data(ItemRole::authorDeathRole).toInt()) {
                        m_model->setData(authorIndex, author->deathYear, ItemRole::authorDeathRole);
                    }
                } else {
                    qWarning() << "Author not found for" << book->title;
                }
            }

            if(bookItems.size()) {
                bookItem->appendRows(bookItems);
                m_updateBooksModel->appendRow(bookItem);
            } else {
                delete bookItem;
            }
        } else {
            qWarning() << "BookListManagerWidget::updateItem book not found:"
                       << index.data().toString()
                       << "remove it from the list";

            if(m_model->removeRow(index.row(), index.parent())) {
                bookItem->setText(tr("حذف %1").arg(bookItem->text()));
                bookItem->setIcon(QIcon(":/images/delete2.png"));
                m_updateBooksModel->appendRow(bookItem);
            } else {
                qWarning() << "BookListManagerWidget::updateItem Can't remove"
                           << index.data().toString() << "from books list";
            }
        }

    }

    QModelIndex child = index.child(0, 0);
    while(child.isValid()) {
        updateItem(child);

        child = child.sibling(child.row()+1, child.column());
    }
}
