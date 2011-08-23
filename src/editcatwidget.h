#ifndef EDITCATWIDGET_H
#define EDITCATWIDGET_H

#include <QWidget>
#include "indexdb.h"
#include "editablebookslistmodel.h"

namespace Ui {
    class EditCatWidget;
}

class EditCatWidget : public QWidget
{
    Q_OBJECT

public:
    EditCatWidget(QWidget *parent = 0);
    ~EditCatWidget();

protected slots:
    void cutNode();
    void pastNode();
    void pastSublingNode();
    void moveUp();
    void moveDown();
    void addCat();
    void removeCat();
    void moveCatBooks();

protected slots:
    void menuRequested(QPoint);
    void updateActions();

private:
    IndexDB *m_indexDB;
    EditableBooksListModel *m_catsModel;
    BooksListNode *m_copiedNode;
    Ui::EditCatWidget *ui;
};

#endif // EDITCATWIDGET_H
