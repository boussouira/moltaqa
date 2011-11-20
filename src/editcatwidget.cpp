#include "editcatwidget.h"
#include "ui_editcatwidget.h"
#include "mainwindow.h"
#include "bookslistmodel.h"
#include <QModelIndex>
#include <qmessagebox.h>
#include <qdebug.h>
#include <qmenu.h>
#include <qaction.h>
#include <qinputdialog.h>
#include "selectcatdialog.h"

EditCatWidget::EditCatWidget(QWidget *parent) :
    AbstractEditWidget(parent),
    ui(new Ui::EditCatWidget)
{
    ui->setupUi(this);

    m_libraryManager = MW->libraryManager();
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_copiedNode = 0;
    m_catsModel = 0;

    loadModel();
    updateActions();

    connect(ui->toolAddCat, SIGNAL(clicked()), SLOT(addCat()));
    connect(ui->toolRemoveCat, SIGNAL(clicked()), SLOT(removeCat()));
    connect(ui->toolMoveUp, SIGNAL(clicked()), SLOT(moveUp()));
    connect(ui->toolMoveDown, SIGNAL(clicked()), SLOT(moveDown()));
    connect(ui->toolMoveRight, SIGNAL(clicked()), SLOT(moveRight()));
    connect(ui->toolMoveLeft, SIGNAL(clicked()), SLOT(moveLeft()));
    connect(ui->treeView, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(menuRequested(QPoint)));
    connect(ui->treeView->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(updateActions()));
    connect(m_catsModel, SIGNAL(layoutChanged()), SLOT(updateActions()));
    connect(m_catsModel, SIGNAL(layoutChanged()), SLOT(modelEdited()));
}

EditCatWidget::~EditCatWidget()
{
    if(m_catsModel)
        delete m_catsModel;

    delete ui;
}

void EditCatWidget::loadModel()
{
    m_catsModel = m_libraryManager->editCatsListModel();
    m_catsModel->setLibraryManager(m_libraryManager);

    ui->treeView->setModel(m_catsModel);
    ui->treeView->resizeColumnToContents(0);
}

void EditCatWidget::save()
{
    m_libraryManager->commit();
}

void EditCatWidget::beginEdit()
{
    m_libraryManager->transaction();
}

void EditCatWidget::cutNode()
{
    if(ui->treeView->selectionModel()->selectedIndexes().isEmpty())
        return;

    if(!m_copiedNode) {
        QModelIndex index = ui->treeView->selectionModel()->selectedIndexes().first();
        m_copiedNode = m_catsModel->nodeFromIndex(index);
        m_catsModel->removeRow(index.row(), index.parent());

        ui->treeView->expand(index);
    }
}

void EditCatWidget::pastNode()
{
    if(ui->treeView->selectionModel()->selectedIndexes().isEmpty())
        return;

    if(m_copiedNode) {
        QModelIndex parentIndex = ui->treeView->selectionModel()->selectedIndexes().first();
        int row = m_catsModel->appendNode(m_copiedNode, parentIndex);
        ui->treeView->expand(parentIndex);

        ui->treeView->scrollTo(parentIndex.child(row, 0));
        ui->treeView->selectionModel()->setCurrentIndex(parentIndex.child(row, 0),
                                                        QItemSelectionModel::ClearAndSelect);
        m_copiedNode = 0;
    }
}

void EditCatWidget::pastSublingNode()
{
    if(ui->treeView->selectionModel()->selectedIndexes().isEmpty())
        return;

    if(m_copiedNode) {
        QModelIndex parentIndex = ui->treeView->selectionModel()->selectedIndexes().first();
        int row = m_catsModel->appendNode(m_copiedNode, parentIndex.row()+1, parentIndex.parent());
        ui->treeView->expand(parentIndex.parent());

        ui->treeView->scrollTo(parentIndex.sibling(row, 0));
        ui->treeView->selectionModel()->setCurrentIndex(parentIndex.sibling(row, 0),
                                                        QItemSelectionModel::ClearAndSelect);
        m_copiedNode = 0;
    }
}

void EditCatWidget::moveRight()
{
    if(ui->treeView->selectionModel()->selectedIndexes().isEmpty())
        return;

    QModelIndex index = ui->treeView->selectionModel()->selectedIndexes().first();
    QModelIndex parent = index.parent();

    if(index.isValid() && parent.isValid()) {
        BooksListNode *copiedNode = m_catsModel->nodeFromIndex(index);
        m_catsModel->removeRow(index.row(), index.parent());

        int row = m_catsModel->appendNode(copiedNode, parent.row()+1, parent.parent());
        ui->treeView->scrollTo(parent.sibling(row, 0));
        ui->treeView->selectionModel()->setCurrentIndex(parent.sibling(row, 0),
                                                        QItemSelectionModel::ClearAndSelect);
    }
}

void EditCatWidget::moveLeft()
{
    if(ui->treeView->selectionModel()->selectedIndexes().isEmpty())
        return;

    QModelIndex index = ui->treeView->selectionModel()->selectedIndexes().first();
    QModelIndex parent = index.sibling(index.row()-1, 0);

    if(index.isValid() && parent.isValid()) {
        BooksListNode *copiedNode = m_catsModel->nodeFromIndex(index);
        m_catsModel->removeRow(index.row(), index.parent());

        int row = m_catsModel->appendNode(copiedNode, parent);

        ui->treeView->scrollTo(parent.child(row, 0));
        ui->treeView->selectionModel()->setCurrentIndex(parent.child(row, 0),
                                                        QItemSelectionModel::ClearAndSelect);
    }
}

void EditCatWidget::moveCatBooks()
{
    if(ui->treeView->selectionModel()->selectedIndexes().isEmpty())
        return;

    selectCatDialog dialog(this);

    if(dialog.exec() == QDialog::Accepted) {
        QModelIndex index = ui->treeView->selectionModel()->selectedIndexes().first();
        BooksListNode *fromNode = m_catsModel->nodeFromIndex(index);
        BooksListNode *toNode = dialog.selectedNode();

        if(fromNode->id == toNode->id)
            return;

        int rep = QMessageBox::question(this,
                                        tr("حذف قسم"),
                                        tr("هل انت متأكد من أنك تريد نقل الكتب الموجودة في قسم '%1' الى قسم '%2'")
                                        .arg(fromNode->title)
                                        .arg(toNode->title),
                                        QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
        if(rep == QMessageBox::Yes) {
            if(m_libraryManager->moveCatBooks(fromNode->id, toNode->id)) {
                QMessageBox::information(this,
                                         tr("نقل كتب القسم"),
                                         tr("تم نقل الكتب الموجودة في قسم '%1' الى قسم '%2'")
                                         .arg(fromNode->title)
                                         .arg(toNode->title));
            } else {
                QMessageBox::warning(this,
                                     tr("نقل كتب القسم"),
                                     tr("حدث خطأ أثناء نقل كتب القسم '%1' الى قسم '%2'")
                                     .arg(fromNode->title)
                                     .arg(toNode->title));
            }
        }
    }
}

void EditCatWidget::moveUp()
{
    if(ui->treeView->selectionModel()->selectedIndexes().isEmpty())
        return;

    QModelIndex fromIndex = ui->treeView->selectionModel()->selectedIndexes().first();
    QModelIndex toIndex = fromIndex.sibling(fromIndex.row()-1, fromIndex.column());
    if(toIndex.isValid()) {
        ui->treeView->collapse(fromIndex);
        ui->treeView->collapse(toIndex);
        ui->treeView->scrollTo(toIndex, QAbstractItemView::EnsureVisible);

        m_catsModel->moveUp(fromIndex);
        ui->treeView->selectionModel()->setCurrentIndex(toIndex, QItemSelectionModel::ClearAndSelect);
    }
}

void EditCatWidget::moveDown()
{
    if(ui->treeView->selectionModel()->selectedIndexes().isEmpty())
        return;

    QModelIndex fromIndex = ui->treeView->selectionModel()->selectedIndexes().first();
    QModelIndex toIndex = fromIndex.sibling(fromIndex.row()+1, fromIndex.column());
    if(toIndex.isValid()) {
        ui->treeView->collapse(fromIndex);
        ui->treeView->collapse(toIndex);
        ui->treeView->scrollTo(toIndex, QAbstractItemView::EnsureVisible);

        m_catsModel->moveDown(fromIndex);
        ui->treeView->selectionModel()->setCurrentIndex(toIndex, QItemSelectionModel::ClearAndSelect);
    }
}

void EditCatWidget::addCat()
{
    bool ok;
    QModelIndex parent;
    if(!ui->treeView->selectionModel()->selectedIndexes().isEmpty())
        parent = ui->treeView->selectionModel()->selectedIndexes().first();

    QString text = QInputDialog::getText(this, tr("اسم القسم"),
                                         tr("ادخل اسم القسم:"), QLineEdit::Normal,
                                         QString(), &ok);
    if(ok && !text.isEmpty()) {
        m_catsModel->addCat(parent, text);

        if(parent.isValid())
            ui->treeView->expand(parent);
    }
}

void EditCatWidget::removeCat()
{
    if(ui->treeView->selectionModel()->selectedIndexes().isEmpty())
        return;

    QModelIndex index = ui->treeView->selectionModel()->selectedIndexes().first();
    BooksListNode *node = m_catsModel->nodeFromIndex(index);
    if(node->childrenNode.count() > 0) {
        QMessageBox::warning(this,
                             tr("حذف القسم"),
                             tr("يجب ان تحذف كل الاقسام الفرعية لقسم '%1' قبل حذفه").arg(node->title));
        return;
    }

    if(m_catsModel->hasBooks(node->id)) {
        QMessageBox::warning(this,
                             tr("حذف القسم"),
                             tr("يجب ان تنقل الكتب الموجودة في هذا القسم الى قسم اخر قبل حذفه"));
        return;
    }

    int rep;
    rep = QMessageBox::question(this,
                                tr("حذف قسم"),
                                tr("هل انت متأكد من أنك تريد حذف قسم '%1'").arg(node->title),
                                QMessageBox::Yes|QMessageBox::No, QMessageBox::No);

    if(rep == QMessageBox::Yes) {
        m_catsModel->removeCat(index);
    }
}

void EditCatWidget::menuRequested(QPoint)
{
    QMenu menu(this);
    QAction *cutAct = new QAction(tr("قص"), &menu);
    QAction *pastAct = new QAction(tr("لصق"), &menu);
    QAction *pastSublingAct = new QAction(tr("لصق في نفس المستوى"), &menu);
    QAction *moveAct = new QAction(tr("نقل كتب هذا القسم الى قسم اخر..."), &menu);

    cutAct->setEnabled(!m_copiedNode);
    pastAct->setEnabled(m_copiedNode);
    pastSublingAct->setEnabled(m_copiedNode);
    menu.addAction(cutAct);
    menu.addAction(pastAct);
    menu.addAction(pastSublingAct);
    menu.addSeparator();
    menu.addAction(moveAct);

    QAction *ret = menu.exec(QCursor::pos());
    if(ret) {
        if(ret == cutAct) {
            cutNode();
        } else if(ret == pastAct) {
            pastNode();
        } else if(ret == pastSublingAct) {
            pastSublingNode();
        } else if(ret == moveAct) {
            moveCatBooks();
        }
    }
}

void EditCatWidget::updateActions()
{
    if(!m_catsModel)
        return;

    if(ui->treeView->selectionModel()->selectedIndexes().isEmpty()) {
        ui->toolMoveDown->setEnabled(false);
        ui->toolMoveUp->setEnabled(false);
        ui->toolMoveLeft->setEnabled(false);
        ui->toolMoveRight->setEnabled(false);
        ui->toolRemoveCat->setEnabled(false);
    } else {
        QModelIndex index = ui->treeView->selectionModel()->selectedIndexes().first();
        QModelIndex prevIndex = index.sibling(index.row()+1, index.column());
        QModelIndex nextIndex = index.sibling(index.row()-1, index.column());

        ui->toolMoveUp->setEnabled(nextIndex.isValid());
        ui->toolMoveDown->setEnabled(prevIndex.isValid());
        ui->toolRemoveCat->setEnabled(index.isValid());
        ui->toolMoveRight->setEnabled(index.parent().isValid());
        ui->toolMoveLeft->setEnabled(index.sibling(index.row()-1, 0).isValid());
    }
}

void EditCatWidget::modelEdited()
{
    emit edited(true);
}
