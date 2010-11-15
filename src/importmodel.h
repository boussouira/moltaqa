#ifndef IMPORTMODEL_H
#define IMPORTMODEL_H

#include "bookinfo.h"
#include <qobject.h>
#include <qabstractitemmodel.h>
#include <qicon.h>
#include <qbrush.h>

class ImportModelNode
{
public:
    ImportModelNode(BookInfo::Type pType, QString pName = "", QString pAuth = "", int pId = 0);
    ~ImportModelNode();
    void appendChild(ImportModelNode *pNode);
    void deleteChild(int index);
    void setID(int pId);

    BookInfo::Type nodeType() const { return m_type; }
    ImportModelNode *parentNode() const { return m_parentNode; }
    QList<ImportModelNode *> childs() const { return m_childrenNode; }
    int id() const { return m_nodeID; }
    QString bookName() const { return m_bookName; }
    QString authorName() const { return m_authorName; }
    QString bookInfo() { return m_bookInfo; }
    QString bookPath() { return m_bookPath; }
    QString typeName() { return m_typeName; }
    QString catName() { return m_catName; }
    int catID() { return m_catID; }
    QBrush backgroundColor() { return m_bgColor; }

    void setBookName(const QString &name) { m_bookName = name; }
    void setAuthorName(const QString &name) { m_authorName = name; }
    void setBookInfo(const QString &info) { m_bookInfo = info; }
    void setBookPath(const QString &path) { m_bookPath = path; }
    void setTypeName(const QString &name) { m_typeName = name; }
    void setCatName(const QString &name) { m_catName = name; }
    void setCatID(int id);
    void setBackgroundColor(const QBrush &color);

protected:
    BookInfo::Type m_type;
    QString m_bookName;
    QString m_authorName;
    QString m_bookInfo;
    QString m_bookPath;
    QString m_typeName;
    QString m_catName;
    int m_catID;
    int m_nodeID;
    QBrush m_bgColor;
    ImportModelNode *m_parentNode;
    QList<ImportModelNode *> m_childrenNode;
};

class ImportModel : public QAbstractItemModel
{
public:
    ImportModel(QObject *parent = 0);
    ~ImportModel();
    void setRootNode(ImportModelNode *ImportModelNode);
    QModelIndex index(int row, int column,
                      const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole );
    Qt::ItemFlags flags ( const QModelIndex & index ) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role) const;
    ImportModelNode *nodeFromIndex(const QModelIndex &index) const;
    void appendNode(ImportModelNode *pNode, const QModelIndex &index);
    void removeRow(int row, const QModelIndex &parent);
    void removeRow(const QList<int> &row, const QModelIndex &parent);

private:
    ImportModelNode *m_rootNode;
};
#endif // IMPORTMODEL_H
