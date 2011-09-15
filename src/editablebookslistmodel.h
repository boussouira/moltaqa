#ifndef EDITABLEBOOKSLISTMODEL_H
#define EDITABLEBOOKSLISTMODEL_H

#include "bookslistmodel.h"
#include "librarymanager.h"

class EditableBooksListModel : public BooksListModel
{
    Q_OBJECT

public:
    EditableBooksListModel(QObject *parent = 0);
    ~EditableBooksListModel();

    bool removeRows(int row, int count, const QModelIndex &parent);
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    void setLibraryManager(LibraryManager *db);
    void setModelEditibale(bool editable);

protected:
    LibraryManager *m_libraryManager;
    bool m_editable;
};

#endif // EDITABLEBOOKSLISTMODEL_H
