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

    void loadModel();
    void save();

protected slots:
    void copyNode();
    void cutNode();
    void pastNode();
    void pastSublingNode();
    void moveUp();
    void moveDown();
    void moveRight();
    void moveLeft();
    void addToBookList();
    void addCat();
    void addBooks();
    void removeCat();
    void modelEdited();

    void menuRequested(QPoint);
    void updateActions();

protected:
    void addBookItem(LibraryBookPtr book, const QModelIndex &parent);

protected:
    QStandardItemModel *m_model;
    QList<QStandardItem*> m_copiedItems;
    BookListManager *m_manager;
    Ui::BookListManagerWidget *ui;
};

#endif // EDITCATWIDGET_H
