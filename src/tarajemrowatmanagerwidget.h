#ifndef TARAJEMROWATMANAGERWIDGET_H
#define TARAJEMROWATMANAGERWIDGET_H

#include "controlcenterwidget.h"
#include "editwebview.h"
#include "tarajemrowatmanager.h"
#include <qstandarditemmodel.h>

namespace Ui {
class TarajemRowatManagerWidget;
}

class ModelViewFilter;

class TarajemRowatManagerWidget : public ControlCenterWidget
{
    Q_OBJECT

public:
    TarajemRowatManagerWidget(QWidget *parent = 0);
    ~TarajemRowatManagerWidget();

    QString title();

    void loadModel();
    void save();

    void aboutToShow();

protected:
    void setupActions();
    void enableEditWidgets(bool enable);
    void saveCurrentRawi();
    RawiInfoPtr getRawiInfo(int rawiID);

private slots:
    void on_treeView_doubleClicked(const QModelIndex &index);
    void infoChanged();
    void checkEditWebChange();
    void birthDeathChanged();
    void newRawi();
    void removeRawi();

protected:
    Ui::TarajemRowatManagerWidget *ui;
    TarajemRowatManager *m_manager;
    QStandardItemModel *m_model;
    EditWebView *m_webEditShoek;
    EditWebView *m_webEditTalamid;
    EditWebView *m_webEditTarjama;
    QHash<int, RawiInfoPtr> m_editedRawiInfo;
    RawiInfoPtr m_currentRawi;
    ModelViewFilter *m_filter;
};

#endif // TARAJEMROWATMANAGERWIDGET_H
