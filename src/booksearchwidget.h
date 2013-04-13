#ifndef BOOKSEARCHWIDGET_H
#define BOOKSEARCHWIDGET_H

#include "librarybook.h"
#include "searchwidget.h"
#include "utils.h"

#include <qsqldatabase.h>
#include <qsqlquery.h>

class BookSearchFilter;
class LibraryBookManager;

class BookSearchWidget : public SearchWidget
{
    Q_OBJECT
public:
    BookSearchWidget(QWidget *parent = 0);
    ~BookSearchWidget();
    
    void init(int bookID=0);

protected:
    SearchFilter *getSearchFilterQuery();

protected:
    LibraryBook::Ptr m_bookInfo;
    LibraryBookManager *m_bookManager;
};

#endif // BOOKSEARCHWIDGET_H
