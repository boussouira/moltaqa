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

protected:
    void open();
    void close();
    void getBookItems(int catID, QStandardItem *catItem);

    void generateLists();
    void getBooks(const QModelIndex &index, int role);
    QList<int> selectedID();
    QList<int> unSelectedID();
    int selectedCount();
    int unSelecCount();

protected:
    QList<int> m_selectedBooks;
    QList<int> m_unSelectedBooks;
};

#endif // LIBRARYSEARCHFILTER_H
