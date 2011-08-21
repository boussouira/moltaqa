#include "editablebookslistmodel.h"

EditableBooksListModel::EditableBooksListModel(QObject *parent) : BooksListModel(parent)
{
    m_editable = false;
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

            setCatTitle(node);

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

bool EditableBooksListModel::appendNode(BooksListNode *node, const QModelIndex &parent)
{
    BooksListNode *parentNode = nodeFromIndex(parent);
    if(parentNode) {
        beginInsertRows(parent, parentNode->childrenNode.count(), parentNode->childrenNode.count());
        parentNode->appendChild(node);
        endInsertRows();

        setCatParent(node);
        setCatOrder(node, node->parentNode->childrenNode.count()-1);

        return true;
    }

    return false;
}

bool EditableBooksListModel::appendNode(BooksListNode *node, int row, const QModelIndex &parent)
{
    BooksListNode *parentNode = nodeFromIndex(parent);
    if(parentNode) {
        beginInsertRows(parent, row, row);
        parentNode->appendChild(row, node);
        endInsertRows();

        setCatParent(node);
        setCatOrder(node, row, true);

        return true;
    }

    return false;
}

void EditableBooksListModel::swap(const QModelIndex &parent, int sourceRow, int destRow)
{
    BooksListNode *parentNode = nodeFromIndex(parent);
    parentNode->childrenNode.swap(sourceRow, destRow);

    emit layoutChanged();
}

void EditableBooksListModel::moveUp(const QModelIndex &index)
{
    QModelIndex toIndex = index.sibling(index.row()-1, index.column());
    if(toIndex.isValid()) {
        swap(index.parent(), index.row(), toIndex.row());

        setCatOrder(nodeFromIndex(index), toIndex.row());
        setCatOrder(nodeFromIndex(toIndex), index.row());
    }
}

void EditableBooksListModel::moveDown(const QModelIndex &index)
{
    QModelIndex toIndex = index.sibling(index.row()+1, index.column());
    if(toIndex.isValid()) {
        swap(index.parent(), toIndex.row(), index.row());

        setCatOrder(nodeFromIndex(toIndex), index.row());
        setCatOrder(nodeFromIndex(index), toIndex.row());
    }
}

void EditableBooksListModel::setIndexDB(IndexDB *db)
{
    m_indexDB = db;
}

void EditableBooksListModel::setModelEditibale(bool editable)
{
    m_editable = editable;
}

void EditableBooksListModel::setCatTitle(BooksListNode *node)
{
    m_indexDB->updateCatTitle(node->id, node->title);
}

void EditableBooksListModel::setCatParent(BooksListNode *node)
{
    m_indexDB->updateCatParent(node->id, node->parentNode->id);
}

void EditableBooksListModel::setCatOrder(BooksListNode *node, int order, bool makeplace)
{
    if(makeplace) {
        m_indexDB->makeCatPlace(node->parentNode->id, order);
    }

    m_indexDB->updateCatOrder(node->id, order);
}

void EditableBooksListModel::addCat(const QModelIndex &parent, const QString &title)
{
    BooksListNode *node = new BooksListNode(BooksListNode::Categorie, title);
    BooksListNode *parentNode = nodeFromIndex(parent);
    parentNode->appendChild(node);

    node->id = m_indexDB->addNewCat(title);

    setCatParent(node);
    setCatOrder(node, parentNode->childrenNode.count()-1);

    emit layoutChanged();
}
