#include "controlcenterdialog.h"
#include "ui_controlcenterdialog.h"
#include "mainwindow.h"
#include "bookslistmodel.h"
#include <QModelIndex>
#include <QDebug>
#include <QMenu>
#include <QAction>

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

    connect(ui->toolMoveUp, SIGNAL(clicked()), SLOT(moveUp()));
    connect(ui->toolMoveDown, SIGNAL(clicked()), SLOT(moveDown()));
    connect(ui->treeView, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(menuRequested(QPoint)));
}

ControlCenterDialog::~ControlCenterDialog()
{
    delete ui;
}

void ControlCenterDialog::cutNode()
{
    if(!m_copiedNode) {
        QModelIndex toIndex = ui->treeView->selectionModel()->currentIndex();
        m_copiedNode = m_catsModel->nodeFromIndex(toIndex);
        m_catsModel->removeRow(toIndex.row(), toIndex.parent());
    }
}

void ControlCenterDialog::pastNode()
{
    if(m_copiedNode) {
        QModelIndex parentIndex = ui->treeView->selectionModel()->currentIndex();
        m_catsModel->appendNode(m_copiedNode, parentIndex);
        ui->treeView->expand(parentIndex);

        m_copiedNode = 0;
    }
}

void ControlCenterDialog::pastSublingNode()
{
    if(m_copiedNode) {
        QModelIndex parentIndex = ui->treeView->selectionModel()->currentIndex();
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
        ui->treeView->scrollTo(toIndex, QAbstractItemView::EnsureVisible);

        m_catsModel->moveDown(fromIndex);
        ui->treeView->selectionModel()->setCurrentIndex(toIndex, QItemSelectionModel::ClearAndSelect);
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
//    menu.addAction(copyAct);
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
