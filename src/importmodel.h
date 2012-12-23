#ifndef IMPORTMODEL_H
#define IMPORTMODEL_H

#include "librarybook.h"
#include <qobject.h>
#include <qabstractitemmodel.h>
#include <qicon.h>
#include <qbrush.h>
#include <qcoreapplication.h>

class ImportModelNode : public LibraryBook
{
    Q_DECLARE_TR_FUNCTIONS(ImportModelNode);

public:
    ImportModelNode(LibraryBook::Type pType);
    ~ImportModelNode();
    void appendChild(ImportModelNode *pNode);
    void deleteChild(int index);

    void setTypeName(const QString &name);
    void setType(LibraryBook::Type bookType);
    void setCategories(int cid, const QString &name=QString());
    void setAuthor(int aid, const QString &name=QString());

    LibraryBookPtr toLibraryBook();

public:
    QString typeName;
    QString catName;
    int catID;
    QBrush bgColor;
    ImportModelNode *parentNode;
    QList<ImportModelNode *> childrenNode;
};

class ImportModel : public QAbstractItemModel
{
    Q_OBJECT
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
    ImportModelNode *nodeFromIndex(const QModelIndex &index=QModelIndex()) const;
    void appendNode(ImportModelNode *pNode, const QModelIndex &index=QModelIndex());
    void removeRow(int row, const QModelIndex &parent);
    void removeRow(const QList<int> &row, const QModelIndex &parent);

private:
    ImportModelNode *m_rootNode;
};
#endif // IMPORTMODEL_H
