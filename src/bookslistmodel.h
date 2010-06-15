#ifndef BOOKSLISTMODEL_H
#define BOOKSLISTMODEL_H

#include <QObject>
#include <QAbstractItemModel>
#include <QIcon>
#include "bookslistnode.h"

class BooksListModel : public QAbstractItemModel
{
public:
    BooksListModel(QObject *parent = 0);
    ~BooksListModel();
    void setRootNode(BooksListNode *BooksListNode);
    QModelIndex index(int row, int column,
                      const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role) const;
    BooksListNode *nodeFromIndex(const QModelIndex &index) const;

private:
    BooksListNode *rootNode;
};

#endif // BOOKSLISTMODEL_H
