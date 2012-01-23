#include "bookslistmodel.h"

BooksListModel::BooksListModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    m_rootNode = 0;
}

BooksListModel::~BooksListModel()
{
    delete m_rootNode;
}

void BooksListModel::setRootNode(BooksListNode *node)
{
    m_rootNode = node;
    reset();
}

QModelIndex BooksListModel::index(int row, int column,
                                const QModelIndex &parent) const
{
    if (!m_rootNode || row < 0 || column < 0)
        return QModelIndex();
    BooksListNode *parentNode = nodeFromIndex(parent);
    BooksListNode *childNode = parentNode->childrenNode.value(row);
    if (!childNode)
        return QModelIndex();
    return createIndex(row, column, childNode);
}

BooksListNode *BooksListModel::nodeFromIndex(const QModelIndex &index) const
{
    if (index.isValid()) {
        return static_cast<BooksListNode *>(index.internalPointer());
    } else {
        return m_rootNode;
    }
}

int BooksListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;
    BooksListNode *parentNode = nodeFromIndex(parent);
    if (!parentNode)
        return 0;
    return parentNode->childrenNode.count();
}

int BooksListModel::columnCount(const QModelIndex & /* parent */) const
{
    return ColumnCount;
}

QModelIndex BooksListModel::parent(const QModelIndex &child) const
{
    BooksListNode *node = nodeFromIndex(child);
    if (!node)
        return QModelIndex();
    BooksListNode *parentNode = node->parentNode;
    if (!parentNode)
        return QModelIndex();
    BooksListNode *grandparentNode = parentNode->parentNode;
    if (!grandparentNode)
        return QModelIndex();
    int row = grandparentNode->childrenNode.indexOf(parentNode);
    return createIndex(row, 0, parentNode);
}

QVariant BooksListModel::data(const QModelIndex &index, int role) const
{
    BooksListNode *node = nodeFromIndex(index);
    if (!node)
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        if (index.column() == BookNameCol){
            return node->title;
        } else if (index.column() == AuthorNameCol) {
            if(node->type == BooksListNode::Book && node->bookType != LibraryBook::QuranBook)
                return node->authorName;
        } else if(index.column() == AuthorDeathCol) {
            if(node->type == BooksListNode::Book && node->bookType != LibraryBook::QuranBook)
                return node->authorDeath;
        }

    } else if (role == Qt::ToolTipRole && node->type == BooksListNode::Book) {
        if (index.column() == BookNameCol)
            return node->infoToolTip;

    } else if (role == Qt::DecorationRole && index.column() == BookNameCol) {
        if(node->type == BooksListNode::Categorie)
            return QIcon(":/images/book-cat.png");
        else if (node->type == BooksListNode::Book)
            return QIcon(":/images/book.png");
    } else if (role == ItemRole::idRole) {
        return node->id;
    } else if (role == ItemRole::orderRole) {
        return node->order;
    } else if (role == ItemRole::authorDeathRole) {
        return node->authorDeathYear;
    }

    return QVariant();
}

QVariant BooksListModel::headerData(int section,
                                  Qt::Orientation orientation,
                                  int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == BookNameCol) {
            return tr("الكتاب");
        } else if (section == AuthorNameCol) {
            return tr("المؤلف");
        } else if (section == AuthorDeathCol) {
            return tr("وفاة المؤلف");
        }
    }

    return QVariant();
}
