#ifndef MODELUTILS_H
#define MODELUTILS_H

#include <qstandarditemmodel.h>

class QTreeView;

namespace Utils {

QModelIndex findModelIndex(QAbstractItemModel *model, int tid);

QStandardItem *itemFromIndex(QStandardItemModel *model, const QModelIndex &index);

QModelIndex changeParent(QStandardItemModel *model, QModelIndex child, QModelIndex newParent, int row=-1);
void swap(QStandardItemModel *model, QModelIndex fromIndex, QModelIndex toIndex);

QModelIndex selectedIndex(QTreeView *tree);
void selectIndex(QTreeView *tree, QModelIndex index);

void moveUp(QStandardItemModel *model, QTreeView *tree);
void moveDown(QStandardItemModel *model, QTreeView *tree);
void moveRight(QStandardItemModel *model, QTreeView *tree);
void moveLeft(QStandardItemModel *model, QTreeView *tree);

QStandardItemModel *cloneModel(QStandardItemModel *model);
QList<QStandardItem *> cloneItem(QStandardItem *item, QStandardItem *parent, int columnCount);
void setModelCheckable(QStandardItemModel *model, bool checkable=true);

}

#endif // MODELUTILS_H
