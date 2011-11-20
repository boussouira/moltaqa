#include "edittaffasirlistwidget.h"
#include "ui_edittaffasirlistwidget.h"
#include "modelenums.h"
#include "mainwindow.h"
#include "librarymanager.h"

typedef QPair<int, QString> Pair;

EditTaffasirListWidget::EditTaffasirListWidget(QWidget *parent) :
    AbstractEditWidget(parent),
    ui(new Ui::EditTaffasirListWidget)
{
    ui->setupUi(this);

    m_libraryManager = MW->libraryManager();
    m_model = 0;
    m_saveChange = true;

    loadModel();
    updateActions();

    connect(ui->toolMoveUp, SIGNAL(clicked()), SLOT(moveUp()));
    connect(ui->toolMoveDown, SIGNAL(clicked()), SLOT(moveDown()));
}

EditTaffasirListWidget::~EditTaffasirListWidget()
{
    if(m_model)
        delete m_model;

    delete ui;
}

void EditTaffasirListWidget::loadModel()
{
    m_model = m_libraryManager->taffessirModel(true);

    ui->treeView->setModel(m_model);
    ui->treeView->resizeColumnToContents(0);

    connect(ui->treeView->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(updateActions()));
    connect(m_model, SIGNAL(layoutChanged()), SLOT(updateActions()));
    connect(m_model, SIGNAL(itemChanged(QStandardItem*)), SLOT(itemChanged(QStandardItem*)));
}

void EditTaffasirListWidget::swap(QModelIndex fromIndex, QModelIndex toIndex)
{
    int f_tafessirID = m_model->item(fromIndex.row(), 0)->data(ItemRole::idRole).toInt();
    QString f_tafessirName = m_model->item(fromIndex.row(), 0)->data(Qt::DisplayRole).toString();
    int f_checked = m_model->item(fromIndex.row(), 1)->data(Qt::CheckStateRole).toInt();

    int t_tafessirID = m_model->item(toIndex.row(), 0)->data(ItemRole::idRole).toInt();
    QString t_tafessirName = m_model->item(toIndex.row(), 0)->data(Qt::DisplayRole).toString();
    int t_checked = m_model->item(toIndex.row(), 1)->data(Qt::CheckStateRole).toInt();

    m_saveChange = false;

    m_model->item(fromIndex.row(), 0)->setData(t_tafessirID, ItemRole::idRole);
    m_model->item(fromIndex.row(), 0)->setData(t_tafessirName, Qt::DisplayRole);
    m_model->item(fromIndex.row(), 1)->setData(t_checked, Qt::CheckStateRole);

    m_model->item(toIndex.row(), 0)->setData(f_tafessirID, ItemRole::idRole);
    m_model->item(toIndex.row(), 0)->setData(f_tafessirName, Qt::DisplayRole);
    m_model->item(toIndex.row(), 1)->setData(f_checked, Qt::CheckStateRole);

    m_saveChange = true;

    m_libraryManager->updateTafessir(f_tafessirID, f_tafessirName, toIndex.row(), f_checked);
    m_libraryManager->updateTafessir(t_tafessirID, t_tafessirName, fromIndex.row(), t_checked);
}

void EditTaffasirListWidget::itemChanged(QStandardItem *item)
{
    if(m_saveChange) {
        int tafessirID = m_model->item(item->row(), 0)->data(ItemRole::idRole).toInt();
        QString tafessirName = m_model->item(item->row(), 0)->data(Qt::DisplayRole).toString();
        int checked = m_model->item(item->row(), 1)->data(Qt::CheckStateRole).toInt();

        m_libraryManager->updateTafessir(tafessirID, tafessirName, item->row(), checked);
        static int count = 0;
        qDebug("itemChanged %d", count++);
    }
}

void EditTaffasirListWidget::save()
{
    m_libraryManager->commit();
}

void EditTaffasirListWidget::beginEdit()
{
    m_libraryManager->transaction();
}

void EditTaffasirListWidget::moveUp()
{
    if(ui->treeView->selectionModel()->selectedIndexes().isEmpty())
        return;

    QModelIndex fromIndex = ui->treeView->selectionModel()->selectedIndexes().first();
    QModelIndex toIndex = fromIndex.sibling(fromIndex.row()-1, fromIndex.column());
    if(toIndex.isValid()) {
        ui->treeView->collapse(fromIndex);
        ui->treeView->collapse(toIndex);
        ui->treeView->scrollTo(toIndex, QAbstractItemView::EnsureVisible);

        swap(fromIndex, toIndex);

        ui->treeView->selectionModel()->setCurrentIndex(toIndex, QItemSelectionModel::ClearAndSelect);
    }
}

void EditTaffasirListWidget::moveDown()
{
    if(ui->treeView->selectionModel()->selectedIndexes().isEmpty())
        return;

    QModelIndex fromIndex = ui->treeView->selectionModel()->selectedIndexes().first();
    QModelIndex toIndex = fromIndex.sibling(fromIndex.row()+1, fromIndex.column());
    if(toIndex.isValid()) {
        ui->treeView->collapse(fromIndex);
        ui->treeView->collapse(toIndex);
        ui->treeView->scrollTo(toIndex, QAbstractItemView::EnsureVisible);

        swap(fromIndex, toIndex);

        ui->treeView->selectionModel()->setCurrentIndex(toIndex, QItemSelectionModel::ClearAndSelect);
    }
}

void EditTaffasirListWidget::updateActions()
{
    if(!m_model || ui->treeView->selectionModel()->selectedIndexes().isEmpty()) {
        ui->toolMoveDown->setEnabled(false);
        ui->toolMoveUp->setEnabled(false);
    } else {
        QModelIndex index = ui->treeView->selectionModel()->selectedIndexes().first();
        QModelIndex prevIndex = index.sibling(index.row()+1, index.column());
        QModelIndex nextIndex = index.sibling(index.row()-1, index.column());

        ui->toolMoveUp->setEnabled(nextIndex.isValid());
        ui->toolMoveDown->setEnabled(prevIndex.isValid());
    }
}
