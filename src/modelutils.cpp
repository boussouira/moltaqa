#include "modelutils.h"
#include "modelenums.h"
#include "utils.h"

#include <qtreeview.h>
#include <qabstractbutton.h>
#include <qmessagebox.h>

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

    if(row == -1 || row > newParentItem->rowCount())
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

void selectIndex(QTreeView *tree, const QModelIndex &index)
{
    if(index.isValid()) {
        tree->scrollTo(index, QAbstractItemView::EnsureVisible);
        tree->selectionModel()->setCurrentIndex(index,
                                                QItemSelectionModel::SelectCurrent|QItemSelectionModel::Rows);
    }
}

bool indexesAtSameLevel(QList<QModelIndex> &list)
{
    ml_return_val_on_fail(list.size(), false);

    if(list.size()==1)
        return true;

    QModelIndex parent = list.first().parent();
    for(int i=0;i<list.size();i++) {
        if(list[i].parent() != parent)
            return false;
    }

    return true;
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

    for(int i=0; i<columnCount; i++) {
        QStandardItem *header = model->horizontalHeaderItem(i);

        if(header)
            newModel->setHorizontalHeaderItem(i, header->clone());
    }

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

TreeViewEditor::TreeViewEditor(QObject *parent) : QObject(parent)
{
     m_tree = 0;
     m_model = 0;
     m_moveUp = 0;
     m_moveDown = 0;
     m_moveLeft = 0;
     m_moveRight = 0;
     m_remove = 0;
     m_lastLeftRow = -1;
     m_dataChanged = false;
}

TreeViewEditor::~TreeViewEditor()
{
}

void TreeViewEditor::setTreeView(QTreeView *tree)
{
    ml_return_on_fail(tree);
    ml_return_on_fail(tree->selectionModel());

    m_tree = tree;
}

void TreeViewEditor::setModel(QStandardItemModel *model)
{
    ml_return_on_fail(model);

    m_model = model;
    m_dataChanged = false;
}

void TreeViewEditor::setup()
{
    ml_return_on_fail2(m_model, "TreeViewEditor::setup model is null");
    ml_return_on_fail2(m_tree, "TreeViewEditor::setup tree view is null");
    ml_return_on_fail2(m_tree->selectionModel(), "TreeViewEditor::setup selection model is null");

    m_tree->setSelectionMode(QAbstractItemView::ExtendedSelection);

    // Detect model editing
    connect(m_model, SIGNAL(itemChanged(QStandardItem*)), SLOT(dataChanged()));
    connect(m_model, SIGNAL(rowsInserted(QModelIndex, int, int)), SLOT(dataChanged()));
    connect(m_model, SIGNAL(rowsRemoved(QModelIndex, int, int)), SLOT(dataChanged()));
    connect(m_model, SIGNAL(rowsMoved(QModelIndex, int, int, QModelIndex, int)), SLOT(dataChanged()));

    connect(m_model,
            SIGNAL(layoutChanged()),
            SLOT(updateActions()));

    connect(m_tree->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(updateActions()));
}

void TreeViewEditor::setMoveUpButton(QAbstractButton *btn)
{
    ml_return_on_fail2(btn, "TreeViewEditor::setMoveUpButton Button is null");

    m_moveUp = btn;
    connect(m_moveUp, SIGNAL(clicked()), SLOT(moveUp()));
}

void TreeViewEditor::setMoveDownButton(QAbstractButton *btn)
{
    ml_return_on_fail2(btn, "TreeViewEditor::setMoveDownButton Button is null");

    m_moveDown = btn;
    connect(m_moveDown, SIGNAL(clicked()), SLOT(moveDown()));
}

void TreeViewEditor::setMoveLeftButton(QAbstractButton *btn)
{
    ml_return_on_fail2(btn, "TreeViewEditor::setMoveLeftButton Button is null");

    m_moveLeft = btn;
    connect(m_moveLeft, SIGNAL(clicked()), SLOT(moveLeft()));
}

void TreeViewEditor::setMoveRightButton(QAbstractButton *btn)
{
    ml_return_on_fail2(btn, "TreeViewEditor::setMoveRightButton Button is null");

    m_moveRight = btn;
    connect(m_moveRight, SIGNAL(clicked()), SLOT(moveRight()));
}

void TreeViewEditor::setRemovButton(QAbstractButton *btn)
{
    ml_return_on_fail2(btn, "TreeViewEditor::setRemovButton Button is null");

    m_remove = btn;
    connect(m_remove, SIGNAL(clicked()), SLOT(removeItem()));
}

void TreeViewEditor::dataChanged()
{
    m_dataChanged = true;

    emit modelDataChanged();
}

void TreeViewEditor::updateActions()
{
    ml_return_on_fail(m_model);
    ml_return_on_fail(m_tree->selectionModel());

    QModelIndexList list = m_tree->selectionModel()->selectedRows();
    qSort(list);

    bool sameLevel = Utils::Model::indexesAtSameLevel(list);

    QModelIndex index = (list.size() ? list.first() : QModelIndex());

    if(m_moveUp)
        m_moveUp->setEnabled(sameLevel);

    if(m_moveDown)
        m_moveDown->setEnabled(sameLevel);

    if(m_moveRight)
        m_moveRight->setEnabled(index.parent().isValid() || list.size());

    if(m_moveLeft)
        m_moveLeft->setEnabled(index.sibling(index.row()-1, 0).isValid() || list.size());

    if(m_remove)
        m_remove->setEnabled(index.isValid() && sameLevel);
}

void TreeViewEditor::moveUp()
{
    QModelIndexList list = m_tree->selectionModel()->selectedRows();
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

        m_tree->collapse(index);
        m_tree->collapse(toIndex);
        m_tree->scrollTo(toIndex, QAbstractItemView::EnsureVisible);

        Utils::Model::swap(m_model, index, toIndex);
        selection << toIndex;
    }

    m_tree->selectionModel()->clear();
    qSort(selection);

    if(selection.size()) {
        for(int i=0;i<selection.size();i++) {
            QModelIndex index = selection[i];
            if(index.isValid()) {
                m_tree->selectionModel()->select(index,
                                                 QItemSelectionModel::Select|QItemSelectionModel::Rows);
            }
        }
    }
}

void TreeViewEditor::moveDown()
{
    QModelIndexList list = m_tree->selectionModel()->selectedRows();
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

        m_tree->collapse(index);
        m_tree->collapse(toIndex);
        m_tree->scrollTo(toIndex, QAbstractItemView::EnsureVisible);

        Utils::Model::swap(m_model, index, toIndex);
        selection << toIndex;
    }

    m_tree->selectionModel()->clear();
    qSort(selection);

    if(selection.size()) {
        for(int i=selection.size()-1;i>=0;i--) {
            QModelIndex index = selection[i];
            if(index.isValid()) {
                m_tree->selectionModel()->select(index,
                                                 QItemSelectionModel::Select|QItemSelectionModel::Rows);
            }
        }
    }
}

void TreeViewEditor::moveRight()
{
    QModelIndexList list = m_tree->selectionModel()->selectedRows();
    ml_return_on_fail(list.size());
    qSort(list);

    QList<QStandardItem *> items;
    QModelIndexList selection;

    for(int i=list.size()-1;i>=0;i--) {
        QModelIndex index = list[i];
        QModelIndex parent = index.parent();
        if(!index.isValid() || !parent.isValid()) {
            selection << index;
            continue;
        }

        QModelIndex newParent = parent.parent();

        QModelIndex changedIndex = Utils::Model::changeParent(m_model, index, newParent, parent.row()+1);
        QStandardItem *item = m_model->itemFromIndex(changedIndex);
        if(item)
            items << item;
    }

    foreach (QStandardItem *item, items) {
        QModelIndex index = m_model->indexFromItem(item);
        if(index.isValid())
            selection << index;
    }

    m_tree->selectionModel()->clear();
    qSort(selection);

    if(selection.size()) {
        for(int i=selection.size()-1;i>=0;i--) {
            QModelIndex index = selection[i];
            if(index.isValid()) {
                m_tree->selectionModel()->select(index,
                                                 QItemSelectionModel::Select|QItemSelectionModel::Rows);
            }
        }
    }
}

void TreeViewEditor::moveLeft()
{
    QModelIndexList list = m_tree->selectionModel()->selectedRows();
    ml_return_on_fail(list.size());
    qSort(list);

    QList<QStandardItem *> items;
    QModelIndexList selection;

    for(int i=list.size()-1;i>=0;i--) {
        QModelIndex index = list[i];
        if(!index.isValid())
            continue;

        QModelIndex parent = index.sibling(index.row()-1, index.column());

        while(parent.isValid() && list.contains(parent)) {
            parent = parent.sibling(parent.row()-1, parent.column());
        }

        if(!parent.isValid()) {
            selection << index;
            continue;
        }

        if(m_lastLeftParent != parent) {
            m_lastLeftRow = -1;
        }

        m_lastLeftParent = parent;

        QModelIndex changedIndex = Utils::Model::changeParent(m_model, index, parent, m_lastLeftRow);
        if(m_lastLeftRow == -1)
            m_lastLeftRow = changedIndex.row();

        m_tree->expand(parent);

        QStandardItem *item = m_model->itemFromIndex(changedIndex);
        if(item)
            items << item;
    }

    foreach (QStandardItem *item, items) {
        QModelIndex index = m_model->indexFromItem(item);
        if(index.isValid())
            selection << index;
    }

    m_tree->selectionModel()->clear();
    qSort(selection);

    if(selection.size()) {
        for(int i=0;i<selection.size();i++) {
            QModelIndex index = selection[i];
            if(index.isValid()) {
                m_tree->selectionModel()->select(index,
                                                 QItemSelectionModel::Select|QItemSelectionModel::Rows);
            }
        }
    }

    m_lastLeftParent = QModelIndex();
}

void TreeViewEditor::removeItem()
{
    QModelIndexList list = m_tree->selectionModel()->selectedRows();
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

    int rep = QMessageBox::question(0,
                                    tr("حذف عناصر من الشجرة"),
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
