#ifndef FAVOURITESMANAGERWIDGET_H
#define FAVOURITESMANAGERWIDGET_H

#include "booklistmanagerwidget.h"

namespace Ui {
class FavouritesManagerWidget;
}

class QStandardItemModel;
class QStandardItem;

class FavouritesManagerWidget : public BookListManagerWidget
{
    Q_OBJECT
    
public:
    FavouritesManagerWidget(QWidget *parent = 0);
    ~FavouritesManagerWidget();

    QString title();
};

#endif // FAVOURITESMANAGERWIDGET_H
