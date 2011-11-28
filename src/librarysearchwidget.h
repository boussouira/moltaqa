#ifndef LIBRARYSEARCHWIDGET_H
#define LIBRARYSEARCHWIDGET_H

#include "searchwidget.h"

class LibrarySearchFilter;

class LibrarySearchWidget : public SearchWidget
{
    Q_OBJECT

public:
    LibrarySearchWidget(QWidget *parent = 0);
    ~LibrarySearchWidget();

    void init(int bookID=0);

protected:
    SearchFilter *getSearchFilterQuery();
};

#endif // LIBRARYSEARCHWIDGET_H
