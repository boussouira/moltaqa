#ifndef MODELVIEWFILTER_H
#define MODELVIEWFILTER_H

#include <qobject.h>
#include <qabstractitemmodel.h>
#include <qtreeview.h>
#include "sortfilterproxymodel.h"
#include "fancylineedit.h"
#include <qstandarditemmodel.h>

class ModelViewFilter : public QObject
{
    Q_OBJECT
public:
    ModelViewFilter(QObject *parent = 0);

    void setSourceModel(QStandardItemModel *model);
    void setLineEdit(FancyLineEdit *edit);
    void setTreeView(QTreeView *view);

    void setup();

    SortFilterProxyModel *filterModel();

protected slots:
    void setFilterText(QString text);
    void clearFilter();

protected:
    QTreeView *m_treeView;
    QStandardItemModel *m_model;
    SortFilterProxyModel *m_filterModel;
    FancyLineEdit *m_lineEdit;

    Qt::ItemDataRole m_role;
    int m_filterColumn;
};

#endif // MODELVIEWFILTER_H
