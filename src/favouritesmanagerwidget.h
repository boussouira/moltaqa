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

//    void loadModel();
//    void save();

protected:
//    QStandardItemModel *m_model;
//    QList<QStandardItem*> m_copiedItems;
//    Ui::FavouritesManagerWidget *ui;
};

#endif // FAVOURITESMANAGERWIDGET_H
