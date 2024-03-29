#ifndef TARAJEMROWATVIEW_H
#define TARAJEMROWATVIEW_H

#include "abstarctview.h"
#include "tarajemrowatmanager.h"

namespace Ui {
class TarajemRowatView;
}

class ModelViewFilter;
class QStandardItemModel;
class QModelIndex;

class TarajemRowatView : public AbstarctView
{
    Q_OBJECT
    
public:
    TarajemRowatView(QWidget *parent = 0);
    ~TarajemRowatView();

    static TarajemRowatView *instance();

    QString title();
    QString viewLink();
    void aboutToShow();
    void aboutToHide();

    bool openRawiInfo(int rawiID);

protected:
    int addTab(QString tabText=QString());
    void setCurrentRawi(RawiInfo::Ptr info);
    void setCurrentTabHtml(QString title, QString html);

private slots:
    void on_treeView_doubleClicked(const QModelIndex &index);
    void lastTabClosed();

private:
    TarajemRowatManager *m_rowatManager;
    QStandardItemModel *m_model;
    ModelViewFilter *m_filter;
    RawiInfo::Ptr m_currentRawi;
    Ui::TarajemRowatView *ui;
};

#endif // TARAJEMROWATVIEW_H
