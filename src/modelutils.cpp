#include "modelutils.h"
#include "modelenums.h"

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

QModelIndex Utils::findModelIndex(QAbstractItemModel *model, int tid)
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
