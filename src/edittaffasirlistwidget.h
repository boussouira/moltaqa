#ifndef EDITTAFFASIRLISTWIDGET_H
#define EDITTAFFASIRLISTWIDGET_H

#include "abstracteditwidget.h"
#include <qstandarditemmodel.h>

namespace Ui {
class EditTaffasirListWidget;
}

class LibraryManager;
class TaffesirListManager;

class EditTaffasirListWidget : public AbstractEditWidget
{
    Q_OBJECT

public:
    EditTaffasirListWidget(QWidget *parent = 0);
    ~EditTaffasirListWidget();

protected:
    void loadModel();

public slots:
    void save();
    void beginEdit();
    void updateActions();
    void moveUp();
    void moveDown();

private:
    QStandardItemModel *m_model;
    TaffesirListManager *m_taffesirManager;
    Ui::EditTaffasirListWidget *ui;
};

#endif // EDITTAFFASIRLISTWIDGET_H
