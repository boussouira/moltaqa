#ifndef EDITCATWIDGET_H
#define EDITCATWIDGET_H

#include "abstracteditwidget.h"
#include "librarymanager.h"
#include "editablecatslistmodel.h"

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

protected slots:
    void cutNode();
    void pastNode();
    void pastSublingNode();
    void moveUp();
    void moveDown();
    void addCat();
    void removeCat();
    void moveCatBooks();
    void modelEdited();

    void menuRequested(QPoint);
    void updateActions();

private:
    LibraryManager *m_libraryManager;
    EditableCatsListModel *m_catsModel;
    BooksListNode *m_copiedNode;
    Ui::EditCatWidget *ui;
};

#endif // EDITCATWIDGET_H
