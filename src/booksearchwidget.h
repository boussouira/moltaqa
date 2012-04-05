#ifndef BOOKSEARCHWIDGET_H
#define BOOKSEARCHWIDGET_H

#include "searchwidget.h"
#include "librarybook.h"
#include "utils.h"
#include <qsqldatabase.h>
#include <qsqlquery.h>

class BookSearchFilter;

class BookSearchWidget : public SearchWidget
{
    Q_OBJECT
public:
    BookSearchWidget(QWidget *parent = 0);
    ~BookSearchWidget();
    
    void init(int bookID=0);

protected:
    SearchFilter *getSearchFilterQuery();
    void loadModel();

protected:
    LibraryBookPtr m_bookInfo;
    QSqlDatabase m_bookDB;
    QSqlQuery m_bookQuery;
};

#endif // BOOKSEARCHWIDGET_H
