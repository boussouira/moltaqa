#ifndef BOOKSEARCHFILTER_H
#define BOOKSEARCHFILTER_H

#include "librarybook.h"
#include "quazip.h"
#include "quazipfile.h"
#include "searchfiltermanager.h"

#include <qdom.h>
#include <qfile.h>

class BookSearchFilter : public SearchFilterManager
{
    Q_OBJECT
public:
    BookSearchFilter(QObject *parent = 0);
    ~BookSearchFilter();

    void setLibraryBook(LibraryBook::Ptr book);
    void loadModel();
    SearchFilter *getFilterQuery();

    void setSelectedItems(const QList<int> &ids);
    QList<int> getSelectedItems();

protected:
    void open();
    void close();
    void loadQuranModel(QStandardItemModel *model);
    void loadSimpleBookModel(QStandardItemModel *model);
    void readItem(QDomElement &element, QStandardItem *parent);

    SearchFilter *getQuranFilterQuery();
    SearchFilter *getSimpleBookFilterQuery();
    QList<int> getSelectedSowar();
    QList<int> getSelectedTitles();
    void getChildTitles(const QModelIndex &index, QList<int> &titles);

protected:
    LibraryBook::Ptr m_book;
    QuaZip m_zip;
};

#endif // BOOKSEARCHFILTER_H
