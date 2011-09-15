#include "editablecatslistmodel.h"

EditableCatsListModel::EditableCatsListModel(QObject *parent) : BooksListModel(parent)
{
    m_editable = true;
}

EditableCatsListModel::~EditableCatsListModel()
{
}


bool EditableCatsListModel::removeRows(int row, int count, const QModelIndex &parent)
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

bool EditableCatsListModel::setData(const QModelIndex &index, const QVariant &value, int role)
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


Qt::ItemFlags EditableCatsListModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    if(m_editable) {
        flags |= Qt::ItemIsEditable;
    }

    return flags;
}

QVariant EditableCatsListModel::data(const QModelIndex &index, int role) const
{
    // TODO: return books count
    /*
    if(index.column() == 1 && role == Qt::DisplayRole) {
        return nodeFromIndex(index)->childrenNode.count();
    }
    */

    return BooksListModel::data(index, role);
}

QVariant EditableCatsListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == 0) {
            return tr("القسم");
        } else if (section == 1) {
            return tr("عدد الكتب");
        }
    }
    return QVariant();
}

bool EditableCatsListModel::appendNode(BooksListNode *node, const QModelIndex &parent)
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

bool EditableCatsListModel::appendNode(BooksListNode *node, int row, const QModelIndex &parent)
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

void EditableCatsListModel::swap(const QModelIndex &parent, int sourceRow, int destRow)
{
    BooksListNode *parentNode = nodeFromIndex(parent);
    parentNode->childrenNode.swap(sourceRow, destRow);

    emit layoutChanged();
}

void EditableCatsListModel::moveUp(const QModelIndex &index)
{
    QModelIndex toIndex = index.sibling(index.row()-1, index.column());
    if(toIndex.isValid()) {
        swap(index.parent(), index.row(), toIndex.row());

        setCatOrder(nodeFromIndex(index), toIndex.row());
        setCatOrder(nodeFromIndex(toIndex), index.row());
    }
}

void EditableCatsListModel::moveDown(const QModelIndex &index)
{
    QModelIndex toIndex = index.sibling(index.row()+1, index.column());
    if(toIndex.isValid()) {
        swap(index.parent(), toIndex.row(), index.row());

        setCatOrder(nodeFromIndex(toIndex), index.row());
        setCatOrder(nodeFromIndex(index), toIndex.row());
    }
}

void EditableCatsListModel::setLibraryManager(LibraryManager *db)
{
    m_libraryManager = db;
}

void EditableCatsListModel::setModelEditibale(bool editable)
{
    m_editable = editable;
}

void EditableCatsListModel::setCatTitle(BooksListNode *node)
{
    m_libraryManager->updateCatTitle(node->id, node->title);
}

void EditableCatsListModel::setCatParent(BooksListNode *node)
{
    m_libraryManager->updateCatParent(node->id, node->parentNode->id);
}

void EditableCatsListModel::setCatOrder(BooksListNode *node, int order, bool makeplace)
{
    if(makeplace) {
        m_libraryManager->makeCatPlace(node->parentNode->id, order);
    }

    m_libraryManager->updateCatOrder(node->id, order);
}

void EditableCatsListModel::addCat(const QModelIndex &parent, const QString &title)
{
    BooksListNode *node = new BooksListNode(BooksListNode::Categorie, title);
    BooksListNode *parentNode = nodeFromIndex(parent);
    parentNode->appendChild(node);

    node->id = m_libraryManager->addNewCat(title);

    setCatParent(node);
    setCatOrder(node, parentNode->childrenNode.count()-1);

    emit layoutChanged();
}

void EditableCatsListModel::removeCat(const QModelIndex &index)
{
    BooksListNode *node = nodeFromIndex(index);
    BooksListNode *parentNode = nodeFromIndex(index.parent());

    parentNode->childrenNode.removeAt(index.row());
    m_libraryManager->removeCat(node->id);

//    delete node;

    emit layoutChanged();
}

bool EditableCatsListModel::hasBooks(int catID)
{
    return m_libraryManager->booksCount(catID);
}
