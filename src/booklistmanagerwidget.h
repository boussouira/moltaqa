#ifndef EDITCATWIDGET_H
#define EDITCATWIDGET_H

#include "controlcenterwidget.h"
#include "librarymanager.h"

namespace Ui {
    class BookListManagerWidget;
}

class TreeViewEditor;

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
    void reloadModel();
    void copyNode();
    void cutNode();
    void pastNode();
    void pastSublingNode();
    void addToBookList();
    void addCat();
    void addBooks();
    void modelEdited();

    void menuRequested(QPoint);

protected:
    void addBookItem(LibraryBookPtr book, const QModelIndex &parent);

protected:
    QStandardItemModel *m_model;
    QList<QList<QStandardItem*> > m_copiedItems;
    QList<QList<QStandardItem*> > m_cattedItems;
    BookListManager *m_manager;
    TreeViewEditor *m_treeManager;
    Ui::BookListManagerWidget *ui;
};

#endif // EDITCATWIDGET_H
