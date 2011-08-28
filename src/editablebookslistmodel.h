#ifndef EDITABLEBOOKSLISTMODEL_H
#define EDITABLEBOOKSLISTMODEL_H

#include "bookslistmodel.h"
#include "indexdb.h"

class EditableBooksListModel : public BooksListModel
{
    Q_OBJECT

public:
    EditableBooksListModel(QObject *parent = 0);
    ~EditableBooksListModel();

    bool removeRows(int row, int count, const QModelIndex &parent);
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    void setIndexDB(IndexDB *db);
    void setModelEditibale(bool editable);

protected:
    IndexDB *m_indexDB;
    bool m_editable;
};

#endif // EDITABLEBOOKSLISTMODEL_H
