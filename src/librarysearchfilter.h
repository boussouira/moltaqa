#ifndef LIBRARYSEARCHFILTER_H
#define LIBRARYSEARCHFILTER_H

#include "searchfiltermanager.h"

class LibrarySearchFilter : public SearchFilterManager
{
    Q_OBJECT
public:
    LibrarySearchFilter(QObject *parent = 0);
    
    void loadModel();
    SearchFilter *getFilterQuery();

    void setSelectedItems(const QList<int> &ids);
    QList<int> getSelectedItems();

    void setForceFilter(bool force);

    void generateLists();

    inline QList<int> selectedID() { return m_selectedBooks; }
    inline QList<int> unSelectedID() { return m_unSelectedBooks; }

    inline int selectedCount() { return m_selectedBooks.count(); }
    inline int unSelecCount() { return m_unSelectedBooks.count(); }

protected:
    void getBooks(const QModelIndex &index);
    void addBook(const QModelIndex &index);

    void selectItem(QStandardItem *item, const QList<int> &ids);

protected:
    QList<int> m_selectedBooks;
    QList<int> m_unSelectedBooks;
    bool m_forceFilter; ///< Create filter even if no book or all books are selected
};

#endif // LIBRARYSEARCHFILTER_H
