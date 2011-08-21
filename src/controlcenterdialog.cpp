#include "controlcenterdialog.h"
#include "ui_controlcenterdialog.h"
#include "mainwindow.h"
#include "bookslistmodel.h"
#include <QModelIndex>
#include <qdebug.h>
#include <qmenu.h>
#include <qaction.h>
#include <qinputdialog.h>

ControlCenterDialog::ControlCenterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ControlCenterDialog)
{
    ui->setupUi(this);

    m_indexDB = MainWindow::mainWindow()->indexDB();
    m_catsModel = new EditableBooksListModel(this);
    m_catsModel->setRootNode(m_indexDB->catsListModel()->rootNode);
    m_catsModel->setIndexDB(m_indexDB);
    m_catsModel->setModelEditibale(true);

    m_copiedNode = 0;

    ui->treeView->setModel(m_catsModel);
    ui->treeView->setColumnHidden(1, true);
    ui->treeView->resizeColumnToContents(0);
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    updateActions();

    connect(ui->toolAddCat, SIGNAL(clicked()), SLOT(addCat()));
    connect(ui->toolMoveUp, SIGNAL(clicked()), SLOT(moveUp()));
    connect(ui->toolMoveDown, SIGNAL(clicked()), SLOT(moveDown()));
    connect(ui->treeView, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(menuRequested(QPoint)));
    connect(ui->treeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(updateActions()));
    connect(m_catsModel, SIGNAL(layoutChanged()), SLOT(updateActions()));
}

ControlCenterDialog::~ControlCenterDialog()
{
    delete ui;
}

void ControlCenterDialog::cutNode()
{
    if(ui->treeView->selectionModel()->selectedIndexes().isEmpty())
        return;

    if(!m_copiedNode) {
        QModelIndex toIndex = ui->treeView->selectionModel()->selectedIndexes().first();
        m_copiedNode = m_catsModel->nodeFromIndex(toIndex);
        m_catsModel->removeRow(toIndex.row(), toIndex.parent());
    }
}

void ControlCenterDialog::pastNode()
{
    if(ui->treeView->selectionModel()->selectedIndexes().isEmpty())
        return;

    if(m_copiedNode) {
        QModelIndex parentIndex = ui->treeView->selectionModel()->selectedIndexes().first();
        m_catsModel->appendNode(m_copiedNode, parentIndex);
        ui->treeView->expand(parentIndex);

        m_copiedNode = 0;
    }
}

void ControlCenterDialog::pastSublingNode()
{
    if(ui->treeView->selectionModel()->selectedIndexes().isEmpty())
        return;

    if(m_copiedNode) {
        QModelIndex parentIndex = ui->treeView->selectionModel()->selectedIndexes().first();
        m_catsModel->appendNode(m_copiedNode, parentIndex.row()+1, parentIndex.parent());
        ui->treeView->expand(parentIndex.parent());

        m_copiedNode = 0;
    }
}

void ControlCenterDialog::moveUp()
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

void ControlCenterDialog::moveDown()
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

void ControlCenterDialog::addCat()
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

void ControlCenterDialog::menuRequested(QPoint)
{
    QMenu menu(this);
    //QAction *copyAct = new QAction(tr("نسخ"), &menu);
    QAction *cutAct = new QAction(tr("قص"), &menu);
    QAction *pastAct = new QAction(tr("لصق"), &menu);
    QAction *pastSublingAct = new QAction(tr("لصق في نفس المستوى"), &menu);

    cutAct->setEnabled(!m_copiedNode);
    pastAct->setEnabled(m_copiedNode);
    pastSublingAct->setEnabled(m_copiedNode);
    //menu.addAction(copyAct);
    menu.addAction(cutAct);
    menu.addAction(pastAct);
    menu.addAction(pastSublingAct);

    QAction *ret = menu.exec(QCursor::pos());
    if(ret) {
        if(ret == cutAct) {
            cutNode();
        } else if(ret == pastAct) {
            pastNode();
        } else if(ret == pastSublingAct) {
            pastSublingNode();
        }
    }
}

void ControlCenterDialog::updateActions()
{
    if(ui->treeView->selectionModel()->selectedIndexes().isEmpty()) {
        ui->toolMoveDown->setEnabled(false);
        ui->toolMoveUp->setEnabled(false);
        ui->toolRemoveCat->setEnabled(false);
    } else {
        QModelIndex index = ui->treeView->selectionModel()->selectedIndexes().first();
        QModelIndex prevIndex = index.sibling(index.row()+1, index.column());
        QModelIndex nextIndex = index.sibling(index.row()-1, index.column());

        ui->toolMoveUp->setEnabled(nextIndex.isValid());
        ui->toolMoveDown->setEnabled(prevIndex.isValid());
        ui->toolRemoveCat->setEnabled(m_catsModel->nodeFromIndex(index)->childrenNode.isEmpty());
    }
}
