#ifndef EDITTAFFASIRLISTWIDGET_H
#define EDITTAFFASIRLISTWIDGET_H

#include "controlcenterwidget.h"
#include <qstandarditemmodel.h>

namespace Ui {
class TaffesirListManagerWidget;
}

class LibraryManager;
class TaffesirListManager;

class TaffesirListManagerWidget : public ControlCenterWidget
{
    Q_OBJECT

public:
    TaffesirListManagerWidget(QWidget *parent = 0);
    ~TaffesirListManagerWidget();

    QString title();

protected:
    void loadModel();

public slots:
    void save();
    void updateActions();
    void moveUp();
    void moveDown();

private:
    QStandardItemModel *m_model;
    TaffesirListManager *m_taffesirManager;
    Ui::TaffesirListManagerWidget *ui;
};

#endif // EDITTAFFASIRLISTWIDGET_H
