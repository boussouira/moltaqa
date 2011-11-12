#include "bookindexmodel.h"
#include "modelenums.h"

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
    if(!parentNode)
        return QModelIndex();

    if(parentNode->childs.size() <= row)
        return QModelIndex();

    BookIndexNode *childNode = parentNode->childs.at(row);
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
    return parentNode->childs.count();
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
    BookIndexNode *parentNode = node->parentNode;
    if (!parentNode)
        return QModelIndex();
    BookIndexNode *grandparentNode = parentNode->parentNode;
    if (!grandparentNode)
        return QModelIndex();
    int row = grandparentNode->childs.indexOf(parentNode);
    return createIndex(row, 0, parentNode);
}

QVariant BookIndexModel::data(const QModelIndex &index, int role) const
{
    BookIndexNode *node = nodeFromIndex(index);
    if (!node)
        return QVariant();

    if(index.column() == 0) {
        if (role == Qt::DisplayRole || role == Qt::ToolTipRole)
            return node->title;
        else if(role == ItemRole::idRole)
            return node->id;
    }

    return QVariant();
}

QVariant BookIndexModel::headerData(int /*section*/,
                                  Qt::Orientation /*orientation*/,
                                  int /*role*/) const
{
    return QVariant();
}
