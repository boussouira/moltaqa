#ifndef EDITCATWIDGET_H
#define EDITCATWIDGET_H

#include "abstracteditwidget.h"
#include "librarymanager.h"

namespace Ui {
    class EditCatWidget;
}

class EditCatWidget : public AbstractEditWidget
{
    Q_OBJECT

public:
    EditCatWidget(QWidget *parent = 0);
    ~EditCatWidget();

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
    LibraryManager *m_libraryManager;
    QStandardItemModel *m_model;
    QList<QStandardItem*> m_copiedItems;
    Ui::EditCatWidget *ui;
};

#endif // EDITCATWIDGET_H
