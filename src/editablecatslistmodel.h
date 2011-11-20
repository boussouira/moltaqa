#ifndef EDITABLECATSLISTMODEL_H
#define EDITABLECATSLISTMODEL_H

#include "bookslistmodel.h"
#include "librarymanager.h"

class EditableCatsListModel : public BooksListModel
{
    Q_OBJECT

public:
    EditableCatsListModel(QObject *parent = 0);
    ~EditableCatsListModel();

    bool removeRows(int row, int count, const QModelIndex &parent);
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                                int role) const;

    int appendNode(BooksListNode *node, const QModelIndex &parent);
    int appendNode(BooksListNode *node, int row, const QModelIndex &parent);
    void swap(const QModelIndex &parent, int sourceRow, int destRow);
    void moveUp(const QModelIndex &index);
    void moveDown(const QModelIndex &index);
    void addCat(const QModelIndex &parent, const QString &title);
    void removeCat(const QModelIndex &index);

    void setLibraryManager(LibraryManager *db);
    void setModelEditibale(bool editable);
    bool hasBooks(int catID);

protected:
    void setCatTitle(BooksListNode *node);
    void setCatParent(BooksListNode *node);
    void setCatOrder(BooksListNode *node, int order, bool makeplace=false);

protected:
    LibraryManager *m_libraryManager;
    bool m_editable;
};


#endif // EDITABLECATSLISTMODEL_H
