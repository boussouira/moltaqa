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

#include <qabstractitemmodel.h>
#include <qmessagebox.h>
#include <qdebug.h>
#include <qmenu.h>
#include <qaction.h>
#include <qinputdialog.h>

BookListManagerWidget::BookListManagerWidget(QWidget *parent) :
    ControlCenterWidget(parent),
    m_model(0),
    ui(new Ui::BookListManagerWidget)
{
    ui->setupUi(this);

    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    m_manager = LibraryManager::instance()->bookListManager();

    updateActions();

    connect(ui->toolAddCat, SIGNAL(clicked()), SLOT(addToBookList()));
    connect(ui->toolRemoveCat, SIGNAL(clicked()), SLOT(removeCat()));
    connect(ui->toolMoveUp, SIGNAL(clicked()), SLOT(moveUp()));
    connect(ui->toolMoveDown, SIGNAL(clicked()), SLOT(moveDown()));
    connect(ui->toolMoveRight, SIGNAL(clicked()), SLOT(moveRight()));
    connect(ui->toolMoveLeft, SIGNAL(clicked()), SLOT(moveLeft()));
    connect(m_manager, SIGNAL(ModelsReady()), SLOT(reloadModel()));

    connect(ui->treeView, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(menuRequested(QPoint)));
}

BookListManagerWidget::~BookListManagerWidget()
{
    ml_delete_check(m_model);

    delete ui;
}

QString BookListManagerWidget::title()
{
    return tr("لائحة الكتب");
}

void BookListManagerWidget::loadModel()
{
    ml_delete_check(m_model);

    m_model = Utils::Model::cloneModel(m_manager->bookListModel());

    ui->treeView->setModel(m_model);
    ui->treeView->resizeColumnToContents(0);

    connect(m_model, SIGNAL(layoutChanged()), SLOT(updateActions()));
    connect(m_model, SIGNAL(layoutChanged()), SLOT(modelEdited()));
    connect(ui->treeView->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(updateActions()));
}

void BookListManagerWidget::save()
{
    m_manager->save(m_model);
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

void BookListManagerWidget::moveUp()
{
    QModelIndexList list = ui->treeView->selectionModel()->selectedRows();
    ml_return_on_fail(list.size());
    qSort(list);

    QModelIndexList selection;

    for(int i=0;i<list.size();i++) {
        QModelIndex index = list[i];
        if(!index.isValid())
            continue;

        QModelIndex toIndex = index.sibling(index.row()-1, index.column());
        if(!toIndex.isValid() || selection.contains(toIndex)) {
            selection << index;
            continue;
        }

        ui->treeView->collapse(index);
        ui->treeView->collapse(toIndex);
        ui->treeView->scrollTo(toIndex, QAbstractItemView::EnsureVisible);

        Utils::Model::swap(m_model, index, toIndex);
        selection << toIndex;
    }

    ui->treeView->selectionModel()->clear();
    qSort(selection);

    if(selection.size()) {
        for(int i=0;i<selection.size();i++) {
            QModelIndex index = selection[i];
            if(index.isValid()) {
                ui->treeView->selectionModel()->select(index,
                                                       QItemSelectionModel::Select|QItemSelectionModel::Rows);
            }
        }
    }
}

void BookListManagerWidget::moveDown()
{
    QModelIndexList list = ui->treeView->selectionModel()->selectedRows();
    ml_return_on_fail(list.size());
    qSort(list);

    QModelIndexList selection;

    for(int i=list.size()-1;i>=0;i--) {
        QModelIndex index = list[i];
        if(!index.isValid())
            continue;

        QModelIndex toIndex = index.sibling(index.row()+1, index.column());
        if(!toIndex.isValid() || selection.contains(toIndex)) {
            selection << index;
            continue;
        }

        ui->treeView->collapse(index);
        ui->treeView->collapse(toIndex);
        ui->treeView->scrollTo(toIndex, QAbstractItemView::EnsureVisible);

        Utils::Model::swap(m_model, index, toIndex);
        selection << toIndex;
    }

    ui->treeView->selectionModel()->clear();
    qSort(selection);

    if(selection.size()) {
        for(int i=selection.size()-1;i>=0;i--) {
            QModelIndex index = selection[i];
            if(index.isValid()) {
                ui->treeView->selectionModel()->select(index,
                                                       QItemSelectionModel::Select|QItemSelectionModel::Rows);
            }
        }
    }
}

void BookListManagerWidget::moveRight()
{
    Utils::Model::moveRight(m_model, ui->treeView);
}

void BookListManagerWidget::moveLeft()
{
    Utils::Model::moveLeft(m_model, ui->treeView);
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
        int parentID = 0;

        QModelIndex parent = Utils::Model::selectedIndex(ui->treeView);
        while(parent.isValid()) {
            if(parent.data(ItemRole::itemTypeRole).toInt() == ItemType::CategorieItem) {
                parentID = parent.data(ItemRole::idRole).toInt();
                break;
            }

            parent = parent.parent();
        }

        foreach (int bookID, books) {
            LibraryBookPtr book = LibraryManager::instance()->bookManager()->getLibraryBook(bookID);
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

void BookListManagerWidget::removeCat()
{
    QModelIndexList list = ui->treeView->selectionModel()->selectedRows();
    ml_return_on_fail(list.size());
    qSort(list);

    QString msg;
    if(list.size()==1) {
        QStandardItem *item = Utils::Model::itemFromIndex(m_model, list.first());
        if(item)
            msg = tr("هل انت متأكد من أنك تريد حذف '%1'؟").arg(item->text());
    } else {
        msg = tr("هل انت متأكد من أنك تريد حذف %1 سطر؟").arg(list.size());
    }

    int rep = QMessageBox::question(this,
                                title(),
                                msg,
                                QMessageBox::Yes|QMessageBox::No, QMessageBox::No);

    ml_return_on_fail(rep == QMessageBox::Yes);

    for(int i=list.size()-1;i>=0;i--) {
        QModelIndex index = list[i];

        QStandardItem *parentItem = Utils::Model::itemFromIndex(m_model, index.parent());
        if(parentItem) {
            parentItem->removeRow(index.row());
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

void BookListManagerWidget::updateActions()
{
    ml_return_on_fail(m_model);

    QModelIndexList list = ui->treeView->selectionModel()->selectedRows();
    qSort(list);

    bool sameLevel = Utils::Model::indexesAtSameLevel(list);

    QModelIndex index = (list.size() ? list.first() : QModelIndex());
    QModelIndex prevIndex = index.sibling(index.row()+1, index.column());
    QModelIndex nextIndex = index.sibling(index.row()-1, index.column());

    ui->toolMoveUp->setEnabled(sameLevel);
    ui->toolMoveDown->setEnabled(sameLevel);
    ui->toolRemoveCat->setEnabled(index.isValid() && sameLevel);
    ui->toolMoveRight->setEnabled(index.parent().isValid() && sameLevel);
    ui->toolMoveLeft->setEnabled(index.sibling(index.row()-1, 0).isValid() && sameLevel);
}

void BookListManagerWidget::addBookItem(LibraryBookPtr book, const QModelIndex &parent)
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
        AuthorInfoPtr auth = LibraryManager::instance()->authorsManager()->getAuthorInfo(book->authorID);
        QString authName;
        int deathYear = 999999;
        QString deathStr;

        if(auth) {
            authName = auth->name;

            if(auth->unknowDeath) {
                deathStr = tr("مجهول");
                deathYear = Utils::Time::unknowDeathYear();
            } else if(auth->isALive) {
                deathStr = tr("معاصر");
                deathYear = Utils::Time::aliveDeathYear();
            } else {
                deathYear = auth->deathYear;
                deathStr = auth->deathStr;
            }
        }

        QStandardItem *authItem = new QStandardItem();
        authItem->setText(authName);
        authItem->setData(book->authorID, ItemRole::authorIdRole);
        rows << authItem;

        QStandardItem *authDeathItem = new QStandardItem();
        authDeathItem->setText(deathStr);
        authDeathItem->setData(deathYear, ItemRole::authorDeathRole);
        rows << authDeathItem;
    }

    QStandardItem *item = Utils::Model::itemFromIndex(m_model, parent);
    item->appendRow(rows);
}

void BookListManagerWidget::modelEdited()
{
    setModified(true);
}
