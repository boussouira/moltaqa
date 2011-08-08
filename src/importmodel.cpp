#include "importmodel.h"

ImportModelNode::ImportModelNode(BookInfo::Type pType, QString pName, QString pAuth, int pId)
{
    m_type = pType;
    m_bookName = pName;
    m_authorName = pAuth;
    m_nodeID = pId;
    m_parentNode = 0;
    m_bgColor = Qt::white;
}

ImportModelNode::~ImportModelNode()
{
    qDeleteAll(m_childrenNode);
}

void ImportModelNode::appendChild(ImportModelNode *pNode)
{
    pNode->m_parentNode = this;
    m_childrenNode.append(pNode);
}

void ImportModelNode::deleteChild(int index)
{
    delete m_childrenNode.takeAt(index);
}

void ImportModelNode::setID(int pId)
{
    m_nodeID = pId;
}

void ImportModelNode::setCatID(int id)
{
     m_catID = id;
     if(id == -1)
         setCatName(tr("-- غير محدد --"));
}
void ImportModelNode::setTypeName(const QString &name)
{
    m_typeName = name;
    QStringList types;
    types << tr("مصحف")
            << tr("تفسير")
            << tr("متن حديث")
            << tr("عادي");

    int index = types.indexOf(name)+1;
    switch(index){
    case 1:
        m_type = BookInfo::QuranBook;
        break;
    case 2:
        m_type = BookInfo::TafessirBook;
        break;
    case 3:
        m_type = BookInfo::HadditBook;
        break;
    case 4:
        m_type = BookInfo::NormalBook;
        break;
    }
}

void ImportModelNode::setBackgroundColor(const QBrush &color)
{
    m_bgColor = color;
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
    delete m_rootNode;
    m_rootNode = node;
    reset();
}

QModelIndex ImportModel::index(int row, int column,
                                const QModelIndex &parent) const
{
    if (!m_rootNode || row < 0 || column < 0)
        return QModelIndex();
    ImportModelNode *parentNode = nodeFromIndex(parent);
    ImportModelNode *childNode = parentNode->childs().value(row);
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
    return parentNode->childs().count();
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
    ImportModelNode *parentNode = node->parentNode();
    if (!parentNode)
        return QModelIndex();
    ImportModelNode *grandparentNode = parentNode->parentNode();
    if (!grandparentNode)
        return QModelIndex();
    int row = grandparentNode->childs().indexOf(parentNode);
    return createIndex(row, 0, parentNode);
}

QVariant ImportModel::data(const QModelIndex &index, int role) const
{
    ImportModelNode *node = nodeFromIndex(index);
    if (!node)
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        if (index.column() == 0)
            return node->bookName();
        else if (index.column() == 1)
            return node->authorName();
        else if (index.column() == 2)
            return node->typeName();
        else if (index.column() == 3)
            return node->catName();

    } else if (role == Qt::ToolTipRole) {
        if (index.column() == 0)
            return node->bookInfo();
    } else if(role == Qt::BackgroundRole) {
        return node->backgroundColor();
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
            node->setBookName(value.toString());
        else if(index.column() == 1)
            node->setAuthorName(value.toString());
        else if(index.column() == 2)
            node->setTypeName(value.toString());
        else if(index.column() == 3)
            node->setCatName(value.toString());
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

