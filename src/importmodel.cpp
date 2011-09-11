#include "importmodel.h"

ImportModelNode::ImportModelNode(BookInfo::Type pType)
{
    type = pType;
    parentNode = 0;
    catID = 0;
    authorName = "XXX";
    catName = tr("-- غير محدد --");
    bgColor = Qt::white;
}

ImportModelNode::~ImportModelNode()
{
    qDeleteAll(childrenNode);
    childrenNode.clear();
}

void ImportModelNode::appendChild(ImportModelNode *pNode)
{
    pNode->parentNode = this;
    childrenNode.append(pNode);
}

void ImportModelNode::deleteChild(int index)
{
    delete childrenNode.takeAt(index);
}

void ImportModelNode::setTypeName(const QString &name)
{
    typeName = name;
    QStringList types;
    types << tr("مصحف")
            << tr("تفسير")
            << tr("عادي");

    int index = types.indexOf(name)+1;
    switch(index){
    case 1:
        type = BookInfo::QuranBook;
        break;
    case 2:
        type = BookInfo::TafessirBook;
        break;
    case 3:
        type = BookInfo::NormalBook;
        break;
    }
}

// The model
ImportModel::ImportModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    m_rootNode = 0;
}

ImportModel::~ImportModel()
{
    delete m_rootNode;
}

void ImportModel::setRootNode(ImportModelNode *node)
{
    beginResetModel();
    ImportModelNode *oldNode = m_rootNode;
    m_rootNode = node;

    if(oldNode)
        delete oldNode;

    endResetModel();
}

QModelIndex ImportModel::index(int row, int column,
                                const QModelIndex &parent) const
{
    if (!m_rootNode || row < 0 || column < 0)
        return QModelIndex();
    ImportModelNode *parentNode = nodeFromIndex(parent);
    ImportModelNode *childNode = parentNode->childrenNode.value(row);
    if (!childNode)
        return QModelIndex();
    return createIndex(row, column, childNode);
}

ImportModelNode *ImportModel::nodeFromIndex(const QModelIndex &index) const
{
    if (index.isValid()) {
        return static_cast<ImportModelNode *>(index.internalPointer());
    } else {
        return m_rootNode;
    }
}

int ImportModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;
    ImportModelNode *parentNode = nodeFromIndex(parent);
    if (!parentNode)
        return 0;
    return parentNode->childrenNode.count();
}

int ImportModel::columnCount(const QModelIndex & /* parent */) const
{
    return 4;
}

QModelIndex ImportModel::parent(const QModelIndex &child) const
{
    ImportModelNode *node = nodeFromIndex(child);
    if (!node)
        return QModelIndex();
    ImportModelNode *parentNode = node->parentNode;
    if (!parentNode)
        return QModelIndex();
    ImportModelNode *grandparentNode = parentNode->parentNode;
    if (!grandparentNode)
        return QModelIndex();
    int row = grandparentNode->childrenNode.indexOf(parentNode);
    return createIndex(row, 0, parentNode);
}

QVariant ImportModel::data(const QModelIndex &index, int role) const
{
    ImportModelNode *node = nodeFromIndex(index);
    if (!node)
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        if (index.column() == 0)
            return node->bookName;
        else if (index.column() == 1)
            return node->authorName;
        else if (index.column() == 2)
            return node->typeName;
        else if (index.column() == 3)
            return node->catName;

    } else if (role == Qt::ToolTipRole) {
        if (index.column() == 0)
            return node->bookInfo;
    } else if(role == Qt::BackgroundRole) {
        return node->bgColor;
    }
    return QVariant();
}

bool ImportModel::setData(const QModelIndex &index, const QVariant &value, int role )
{
    ImportModelNode *node = nodeFromIndex(index);
    if (!node)
        return false;
    if(role == Qt::EditRole || role == Qt::DisplayRole) {
        if(index.column() == 0)
            node->bookName = value.toString();
        else if(index.column() == 1)
            node->authorName = value.toString();
        else if(index.column() == 2)
            node->typeName = value.toString();
        else if(index.column() == 3)
            node->catName = value.toString();
        layoutChanged();
        return true;
    }
    return false;
}

Qt::ItemFlags ImportModel::flags ( const QModelIndex & /*index*/ ) const
{
    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant ImportModel::headerData(int section,
                                  Qt::Orientation orientation,
                                  int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == 0)       return tr("الكتاب");
        else if (section == 1)  return tr("المؤلف");
        else if (section == 2)  return tr("نوع الكتاب");
        else if (section == 3)  return tr("القسم");

    }
    return QVariant();
}

void ImportModel::appendNode(ImportModelNode *pNode, const QModelIndex &index)
{
    ImportModelNode *node = nodeFromIndex(index);
    if (!node)
        return;
    node->appendChild(pNode);
    layoutChanged();
}

void ImportModel::removeRow(int row, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row);
    nodeFromIndex(parent)->deleteChild(row);
    endRemoveRows();
}

