#ifndef BOOKSEARCHFILTER_H
#define BOOKSEARCHFILTER_H

#include "searchfiltermanager.h"
#include "librarybook.h"

class BookSearchFilter : public SearchFilterManager
{
    Q_OBJECT
public:
    BookSearchFilter(QObject *parent = 0);
    ~BookSearchFilter();

    void setLibraryBook(LibraryBook *book);
    void loadModel();
    SearchFilter *getFilterQuery();

protected:
    void open();
    void close();
    void loadQuranModel(QStandardItemModel *model);
    void loadSimpleBookModel(QStandardItemModel *model);
    void childTitles(QStandardItemModel *model, QStandardItem *parentItem, int tid);

    SearchFilter *getQuranFilterQuery();
    SearchFilter *getSimpleBookFilterQuery();
    QList<int> getSelectedSowar();
    QList<int> getSelectedTitles();
    void getChildTitles(const QModelIndex &index, QList<int> &titles);

protected:
    LibraryBook *m_book;
};

#endif // BOOKSEARCHFILTER_H
