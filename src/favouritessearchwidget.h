#ifndef FAVOURITESSEARCHWIDGET_H
#define FAVOURITESSEARCHWIDGET_H

#include "searchwidget.h"

class FavouritesSearchWidget : public SearchWidget
{
    Q_OBJECT

public:
    FavouritesSearchWidget(QWidget *parent = 0);
    ~FavouritesSearchWidget();

    void init(int bookID=0);

protected:
    SearchFilter *getSearchFilterQuery();
};

#endif // FAVOURITESSEARCHWIDGET_H
