#include "bookslistmodel.h"

BooksListModel::BooksListModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    rootNode = 0;
}

BooksListModel::~BooksListModel()
{
    delete rootNode;
}

void BooksListModel::setRootNode(BooksListNode *BooksListNode)
{
    delete rootNode;
    rootNode = BooksListNode;
    reset();
}

QModelIndex BooksListModel::index(int row, int column,
                                const QModelIndex &parent) const
{
    if (!rootNode || row < 0 || column < 0)
        return QModelIndex();
    BooksListNode *parentNode = nodeFromIndex(parent);
    BooksListNode *childNode = parentNode->childrenList().value(row);
    if (!childNode)
        return QModelIndex();
    return createIndex(row, column, childNode);
}

BooksListNode *BooksListModel::nodeFromIndex(const QModelIndex &index) const
{
    if (index.isValid()) {
        return static_cast<BooksListNode *>(index.internalPointer());
    } else {
        return rootNode;
    }
}

int BooksListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;
    BooksListNode *parentNode = nodeFromIndex(parent);
    if (!parentNode)
        return 0;
    return parentNode->childrenList().count();
}

int BooksListModel::columnCount(const QModelIndex & /* parent */) const
{
    return 2;
}

QModelIndex BooksListModel::parent(const QModelIndex &child) const
{
    BooksListNode *node = nodeFromIndex(child);
    if (!node)
        return QModelIndex();
    BooksListNode *parentNode = node->parentNode();
    if (!parentNode)
        return QModelIndex();
    BooksListNode *grandparentNode = parentNode->parentNode();
    if (!grandparentNode)
        return QModelIndex();
    int row = grandparentNode->childrenList().indexOf(parentNode);
    return createIndex(row, 0, parentNode);
}

QVariant BooksListModel::data(const QModelIndex &index, int role) const
{
    BooksListNode *node = nodeFromIndex(index);
    if (!node)
        return QVariant();

    if (role == Qt::DisplayRole) {
        if (index.column() == 0){
            return node->getTitle();
        } else if (index.column() == 1) {
            if((node->getNodeType()==BooksListNode::Categorie) && node->childrenList().count()>0)
                return node->childrenList().count();
            else
                return node->getAuthorName();
        }

    } else if (role == Qt::ToolTipRole && node->getNodeType() == BooksListNode::Book) {
        if (index.column() == 0)
            return node->getInfoToolTip();

    } else if (role == Qt::DecorationRole && index.column() == 0) {
        if(node->getNodeType() == BooksListNode::Categorie)
            return QIcon(":/menu/images/book-cat.png");
        else if (node->getNodeType() == BooksListNode::Book)
            return QIcon(":/menu/images/book.png");
    }
    return QVariant();
}

QVariant BooksListModel::headerData(int section,
                                  Qt::Orientation orientation,
                                  int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == 0) {
            return tr("الكتاب");
        } else if (section == 1) {
            return tr("المؤلف");
        }
    }
    return QVariant();
}
