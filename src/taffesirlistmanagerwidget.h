#ifndef EDITTAFFASIRLISTWIDGET_H
#define EDITTAFFASIRLISTWIDGET_H

#include "controlcenterwidget.h"

namespace Ui {
class TaffesirListManagerWidget;
}

class LibraryManager;
class TaffesirListManager;
class TreeViewEditor;
class ModelViewSearcher;
class QStandardItemModel;

class TaffesirListManagerWidget : public ControlCenterWidget
{
    Q_OBJECT

public:
    TaffesirListManagerWidget(QWidget *parent = 0);
    ~TaffesirListManagerWidget();

    QString title();

    void aboutToShow();

    void loadModel();
    void save();

private:
    QStandardItemModel *m_model;
    TaffesirListManager *m_taffesirManager;
    TreeViewEditor *m_treeManager;
    ModelViewSearcher *m_viewSearcher;
    Ui::TaffesirListManagerWidget *ui;
};

#endif // EDITTAFFASIRLISTWIDGET_H
