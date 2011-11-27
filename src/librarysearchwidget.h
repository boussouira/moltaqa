#ifndef LIBRARYSEARCHWIDGET_H
#define LIBRARYSEARCHWIDGET_H

#include "searchwidget.h"

class LibrarySearchWidget : public SearchWidget
{
    Q_OBJECT

public:
    LibrarySearchWidget(QWidget *parent = 0);
    ~LibrarySearchWidget();

    void init();

public slots:
    void selectAll();
    void unSelectAll();
    void selectVisible();
    void unSelectVisible();
    void expandFilterView();
    void collapseFilterView();

protected:
    lucene::search::Query *getSearchQuery();
    SearchFilter *getSearchFilterQuery();

protected:
    SearchFilterManager *m_filterManager;
};

#endif // LIBRARYSEARCHWIDGET_H
