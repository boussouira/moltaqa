#include "modelutils.h"
#include "modelenums.h"
#include "utils.h"
#include <qtreeview.h>

namespace Utils {
namespace Model {

QModelIndex fitchChild(QModelIndex parent, int tid)
{
    QModelIndex nodeIndex = parent.child(0, 0);
    while(nodeIndex.isValid()) {
        int pid = nodeIndex.data(ItemRole::idRole).toInt();

        if(pid == tid) {
            return nodeIndex;
        } else {
            QModelIndex index = fitchChild(nodeIndex, tid);
            if(index.isValid())
                return index;
        }

        nodeIndex = nodeIndex.sibling(nodeIndex.row()+1, 0);
    }

    return QModelIndex();
}

QModelIndex findModelIndex(QAbstractItemModel *model, int tid)
{
    QModelIndex nodeIndex = model->index(0, 0, QModelIndex());
    while(nodeIndex.isValid()) {
        int pid = nodeIndex.data(ItemRole::idRole).toInt();

        if(pid == tid) {
            return nodeIndex;
        } else {
            QModelIndex nextIndex = model->index(nodeIndex.row()+1, 0, nodeIndex.parent());
            if(!nextIndex.isValid() ||
                    nextIndex.data(ItemRole::idRole).toInt() > tid) {
                QModelIndex index = fitchChild(nodeIndex, tid);
                if(index.isValid())
                    return index;
            }
        }

        nodeIndex = nodeIndex.sibling(nodeIndex.row()+1, 0);
    }

    return QModelIndex();
}

QStandardItem *itemFromIndex(QStandardItemModel *model, const QModelIndex &index)
{
    return index.isValid() ? model->itemFromIndex(index) : model->invisibleRootItem();
}

QModelIndex changeParent(QStandardItemModel *model, QModelIndex child, QModelIndex newParent, int row)
{
    QModelIndex currentParent = child.parent();

    QStandardItem *currentParentItem = Model::itemFromIndex(model, currentParent);
    QList<QStandardItem*> childItems = currentParentItem->takeRow(child.row());

    QStandardItem *newParentItem = Model::itemFromIndex(model, newParent);

    if(!newParentItem)
        return QModelIndex();

    if(row == -1)
        row = newParentItem->rowCount();

    newParentItem->insertRow(row, childItems);

    QModelIndex insertedIndex = newParent.child(row, 0);
    if(!insertedIndex.isValid())
        insertedIndex = model->index(row, 0);

    return insertedIndex;
}

void swap(QStandardItemModel *model, QModelIndex fromIndex, QModelIndex toIndex)
{
    ml_return_on_fail(fromIndex.parent() == toIndex.parent());

    QModelIndex parent = fromIndex.parent();

    QStandardItem *parentItem = Model::itemFromIndex(model, parent);

    ml_return_on_fail(parentItem);

    QList<QStandardItem*> rows = parentItem->takeRow(fromIndex.row());
    parentItem->insertRow(toIndex.row(), rows);
}

QModelIndex selectedIndex(QTreeView *tree)
{
    QModelIndexList selection = tree->selectionModel()->selectedIndexes();

    return selection.isEmpty() ? QModelIndex() : selection.first();
}

void selectIndex(QTreeView *tree, QModelIndex index)
{
    if(index.isValid()) {
        tree->scrollTo(index, QAbstractItemView::EnsureVisible);
        tree->selectionModel()->setCurrentIndex(index,
                                                QItemSelectionModel::ClearAndSelect);
    }
}

void moveUp(QStandardItemModel *model, QTreeView *tree)
{
    QModelIndex index = Model::selectedIndex(tree);
    ml_return_on_fail(index.isValid());

    QModelIndex toIndex = index.sibling(index.row()-1, index.column());
    ml_return_on_fail(toIndex.isValid());

    tree->collapse(index);
    tree->collapse(toIndex);
    tree->scrollTo(toIndex, QAbstractItemView::EnsureVisible);

    Model::swap(model, index, toIndex);
    Model::selectIndex(tree, toIndex);
}

void moveDown(QStandardItemModel *model, QTreeView *tree)
{
    QModelIndex index = Model::selectedIndex(tree);
    ml_return_on_fail(index.isValid());

    QModelIndex toIndex = index.sibling(index.row()+1, index.column());
    ml_return_on_fail(toIndex.isValid());

    tree->collapse(index);
    tree->collapse(toIndex);
    tree->scrollTo(toIndex, QAbstractItemView::EnsureVisible);

    Model::swap(model, index, toIndex);
    Model::selectIndex(tree, toIndex);
}

void moveRight(QStandardItemModel *model, QTreeView *tree)
{
    QModelIndex index = Model::selectedIndex(tree);
    QModelIndex parent = index.parent();
    ml_return_on_fail(index.isValid() && parent.isValid());

    QModelIndex newParent = parent.parent();

    QModelIndex changedIndex = Model::changeParent(model, index, newParent, parent.row()+1);
    Model::selectIndex(tree, changedIndex);
}

void moveLeft(QStandardItemModel *model, QTreeView *tree)
{
    QModelIndex index = Model::selectedIndex(tree);
    ml_return_on_fail(index.isValid());

    QModelIndex parent = index.sibling(index.row()-1, index.column());

    QModelIndex changedIndex = Model::changeParent(model, index, parent);
    Model::selectIndex(tree, changedIndex);
}

QList<QList<QStandardItem*> > getItemChilds(QStandardItem *item, int columnCount)
{
    QList<QList<QStandardItem*> > rowList;

    for(int r=0; r<item->rowCount(); r++) {
        QList<QStandardItem*> columnList;

        for(int c=0; c<columnCount; c++) {
            QStandardItem *child = item->child(r, c);
            if(child) {
                QStandardItem *newItem = child->clone();

                if(!c) {
                    foreach(QList<QStandardItem*> items, getItemChilds(child, columnCount)) {
                        newItem->appendRow(items);
                    }
                }

                columnList << newItem;
            }
        }

        rowList << columnList;
    }

    return rowList;
}

QStandardItemModel *cloneModel(QStandardItemModel *model)
{
    int columnCount = model->columnCount();
    QStandardItemModel *newModel = new QStandardItemModel();

    for(int i=0; i<columnCount; i++)
        newModel->setHorizontalHeaderItem(i, model->horizontalHeaderItem(i)->clone());

    foreach(QList<QStandardItem*> items, getItemChilds(model->invisibleRootItem(), columnCount)) {
        newModel->appendRow(items);
    }

    return newModel;
}

QList<QStandardItem *> cloneItem(QStandardItem *item, QStandardItem *parent, int columnCount)
{
    QList<QStandardItem *> items;
    ml_return_val_on_fail2(item, "cloneItem: item is null", items);
    ml_return_val_on_fail2(parent, "cloneItem: parent item is null", items);

    for(int i=0;i<columnCount; i++) {
        QStandardItem *newItem = parent->child(item->row(), i);
        if(newItem) {
            QStandardItem *clone = newItem->clone();
            foreach (QList<QStandardItem*> items, getItemChilds(newItem, columnCount))
                clone->appendRow(items);

            items << clone;
        }
    }

    return items;
}

void setItemCheckable(QStandardItem *item, bool checkable)
{
    for(int i=0; i<item->rowCount(); i++) {
        QStandardItem *child = item->child(i);
        child->setCheckable(checkable);

        setItemCheckable(child, checkable);
    }
}

void setModelCheckable(QStandardItemModel *model, bool checkable)
{
    QStandardItem *item = model->invisibleRootItem();

    setItemCheckable(item, checkable);
}

}
}
