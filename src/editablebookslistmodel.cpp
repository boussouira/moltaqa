#include "editablebookslistmodel.h"

EditableBooksListModel::EditableBooksListModel(QObject *parent) : BooksListModel(parent)
{
    m_editable = true;
}

EditableBooksListModel::~EditableBooksListModel()
{
}

bool EditableBooksListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    BooksListNode *parentNode = nodeFromIndex(parent);
    if(row+count <= parentNode->childrenNode.count()) {
        beginRemoveRows(parent, row, row+count);
        for(int i=row; i < row+count; i++) {
            parentNode->childrenNode.removeAt(i);
        }
        endRemoveRows();

        return true;
    }

    return false;
}

bool EditableBooksListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && index.row() != index.column()
            && role == Qt::EditRole) {
        QString title = value.toString();

        if(!title.isEmpty()) {
            BooksListNode *node = nodeFromIndex(index);
            node->title = value.toString();

            emit dataChanged(index, index);
            return true;
        }
    }

    return false;
}

Qt::ItemFlags EditableBooksListModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    if(m_editable) {
        flags |= Qt::ItemIsEditable;
    }

    return flags;
}

void EditableBooksListModel::setIndexDB(IndexDB *db)
{
    m_indexDB = db;
}

void EditableBooksListModel::setModelEditibale(bool editable)
{
    m_editable = editable;
}

