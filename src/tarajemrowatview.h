#ifndef TARAJEMROWATVIEW_H
#define TARAJEMROWATVIEW_H

#include "abstarctview.h"
#include "tarajemrowatmanager.h"
#include <qstandarditemmodel.h>

namespace Ui {
class TarajemRowatView;
}

class ModelViewFilter;

class TarajemRowatView : public AbstarctView
{
    Q_OBJECT
    
public:
    TarajemRowatView(QWidget *parent = 0);
    ~TarajemRowatView();

    QString title();
    void aboutToShow();

protected:
    int addTab(QString tabText=QString());
    void setCurrentRawi(RawiInfoPtr info);
    void setCurrentTabHtml(QString title, QString html);

private slots:
    void on_treeView_doubleClicked(const QModelIndex &index);

private:
    TarajemRowatManager *m_rowatManager;
    QStandardItemModel *m_model;
    ModelViewFilter *m_filter;
    Ui::TarajemRowatView *ui;
};

#endif // TARAJEMROWATVIEW_H
