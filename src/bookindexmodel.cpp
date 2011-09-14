#include "bookindexmodel.h"

BookIndexModel::BookIndexModel(QObject *parent) : QAbstractItemModel(parent), m_rootNode(0)
{
    m_rootNode = 0;
}

BookIndexModel::~BookIndexModel()
{
    delete m_rootNode;
}

void BookIndexModel::setRootNode(BookIndexNode *node)
{
    m_rootNode = node;
    reset();
}

QModelIndex BookIndexModel::index(int row, int column,
                                const QModelIndex &parent) const
{
    if (!m_rootNode || row < 0 || column < 0)
        return QModelIndex();
    BookIndexNode *parentNode = nodeFromIndex(parent);
    BookIndexNode *childNode = parentNode->childList()->at(row);
    if (!childNode)
        return QModelIndex();
    return createIndex(row, column, childNode);
}

BookIndexNode *BookIndexModel::nodeFromIndex(const QModelIndex &index) const
{
    if (index.isValid()) {
        return static_cast<BookIndexNode *>(index.internalPointer());
    } else {
        return m_rootNode;
    }
}

int BookIndexModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;
    BookIndexNode *parentNode = nodeFromIndex(parent);
    if (!parentNode)
        return 0;
    return parentNode->childList()->count();
}

int BookIndexModel::columnCount(const QModelIndex & /* parent */) const
{
    return 1;
}

QModelIndex BookIndexModel::parent(const QModelIndex &child) const
{
    BookIndexNode *node = nodeFromIndex(child);
    if (!node)
        return QModelIndex();
    BookIndexNode *parentNode = node->parent();
    if (!parentNode)
        return QModelIndex();
    BookIndexNode *grandparentNode = parentNode->parent();
    if (!grandparentNode)
        return QModelIndex();
    int row = grandparentNode->childList()->indexOf(parentNode);
    return createIndex(row, 0, parentNode);
}

QVariant BookIndexModel::data(const QModelIndex &index, int role) const
{
    BookIndexNode *node = nodeFromIndex(index);
    if (!node)
        return QVariant();

    if (role == Qt::DisplayRole) // there is only one column
        return node->title();
    else if(role == Qt::ToolTipRole)
        return QString("<p>%1</p>").arg(node->title());

    return QVariant();
}

QVariant BookIndexModel::headerData(int /*section*/,
                                  Qt::Orientation /*orientation*/,
                                  int /*role*/) const
{
    return QVariant();
}

