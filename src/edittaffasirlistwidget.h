#ifndef EDITTAFFASIRLISTWIDGET_H
#define EDITTAFFASIRLISTWIDGET_H

#include "abstracteditwidget.h"
#include <qstandarditemmodel.h>

namespace Ui {
class EditTaffasirListWidget;
}

class LibraryManager;

class EditTaffasirListWidget : public AbstractEditWidget
{
    Q_OBJECT

public:
    EditTaffasirListWidget(QWidget *parent = 0);
    ~EditTaffasirListWidget();

protected:
    void loadModel();
    void swap(QModelIndex fromIndex, QModelIndex toIndex);

public slots:
    void save();
    void beginEdit();
    void updateActions();
    void moveUp();
    void moveDown();
    void itemChanged(QStandardItem *item);

private:
    bool m_saveChange;
    QStandardItemModel *m_model;
    LibraryManager *m_libraryManager;
    Ui::EditTaffasirListWidget *ui;
};

#endif // EDITTAFFASIRLISTWIDGET_H
