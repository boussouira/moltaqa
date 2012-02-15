#ifndef EDITCATWIDGET_H
#define EDITCATWIDGET_H

#include "controlcenterwidget.h"
#include "librarymanager.h"

namespace Ui {
    class BookListManagerWidget;
}

class BookListManagerWidget : public ControlCenterWidget
{
    Q_OBJECT

public:
    BookListManagerWidget(QWidget *parent = 0);
    ~BookListManagerWidget();

    QString title();

protected:
    void loadModel();

public slots:
    void save();
    void beginEdit();

protected slots:
    void cutNode();
    void pastNode();
    void pastSublingNode();
    void moveUp();
    void moveDown();
    void moveRight();
    void moveLeft();
    void addCat();
    void removeCat();
    void modelEdited();

    void menuRequested(QPoint);
    void updateActions();

private:
    QStandardItemModel *m_model;
    QList<QStandardItem*> m_copiedItems;
    Ui::BookListManagerWidget *ui;
};

#endif // EDITCATWIDGET_H
