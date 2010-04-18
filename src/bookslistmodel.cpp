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
    BooksListNode *BooksListNode = nodeFromIndex(index);
    if (!BooksListNode)
        return QVariant();

    if (role == Qt::DisplayRole) {
        if (index.column() == 0) {
            /*
        switch (BooksListNode->type) {
        case BooksListNode::Root:
             return tr("Root");
        case BooksListNode::OrExpression:
            return tr("OR Expression");
        case BooksListNode::AndExpression:
            return tr("AND Expression");
        case BooksListNode::NotExpression:
            return tr("NOT Expression");
        case BooksListNode::Atom:
            return tr("Atom");
        case BooksListNode::Identifier:
            return tr("Identifier");
        case BooksListNode::Operator:
            return tr("Operator");
        case BooksListNode::Punctuator:
            return tr("Punctuator");
        default:
            return tr("Unknown");
        }
        */
            return BooksListNode->getTitle();
        } else if (index.column() == 1) {
            return BooksListNode->getAuthorName();
//            return trUtf8("<strong>%1</strong>").arg(BooksListNode->authorName);
        }
    } else if (role == Qt::ToolTipRole) {

        if(BooksListNode->getNodeType() != BooksListNode::Book) return QVariant();

        if (index.column() == 0) {
            return trUtf8("<strong>كتاب : </strong>%1").arg(BooksListNode->getTitle());
        } else if (index.column() == 1) {
            return trUtf8("<strong>اسم المؤلف : </strong>%1").arg(BooksListNode->getAuthorName());
        }
    }
    return QVariant();
}

QVariant BooksListModel::headerData(int section,
                                  Qt::Orientation orientation,
                                  int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == 0) {
            return trUtf8("الكتاب");
        } else if (section == 1) {
            return trUtf8("المؤلف");
        }
    }
    return QVariant();
}
