#ifndef BOOKSLISTMODEL_H
#define BOOKSLISTMODEL_H

#include <qobject.h>
#include <qabstractitemmodel.h>
#include <qicon.h>
#include "bookslistnode.h"
#include "modelenums.h"

class BooksListModel : public QAbstractItemModel
{
public:
    BooksListModel(QObject *parent = 0);
    virtual ~BooksListModel();
    void setRootNode(BooksListNode *BooksListNode);
    QModelIndex index(int row, int column,
                      const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role) const;
    BooksListNode *nodeFromIndex(const QModelIndex &index) const;

    enum {
        BookNameCol,
        AuthorNameCol,
        AuthorDeathCol,
        ColumnCount
    };

public:
    BooksListNode *m_rootNode;
};

#endif // BOOKSLISTMODEL_H
