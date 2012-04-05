#ifndef BOOKSEARCHFILTER_H
#define BOOKSEARCHFILTER_H

#include "searchfiltermanager.h"
#include "librarybook.h"

#include <qdom.h>
#include <qfile.h>

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

class BookSearchFilter : public SearchFilterManager
{
    Q_OBJECT
public:
    BookSearchFilter(QObject *parent = 0);
    ~BookSearchFilter();

    void setLibraryBook(LibraryBookPtr book);
    void loadModel();
    SearchFilter *getFilterQuery();

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
    LibraryBookPtr m_book;
    QuaZip m_zip;
};

#endif // BOOKSEARCHFILTER_H
