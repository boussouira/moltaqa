#ifndef BOOKINDEXMODEL_H
#define BOOKINDEXMODEL_H

#include <QAbstractItemModel>
#include "bookindexnode.h"

class BookIndexModel : public QAbstractItemModel
{
public:
    BookIndexModel(QObject *parent=0) ;
    ~BookIndexModel();
    void setRootNode(BookIndexNode *node);
    QModelIndex index(int row, int column,
                      const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role) const;
    BookIndexNode *nodeFromIndex(const QModelIndex &index) const;
private:
    BookIndexNode *rootNode;
};

#endif // BOOKINDEXMODEL_H
