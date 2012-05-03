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

    void setDefautSortRole(int role);
    void setDefautSortColumn(int column, Qt::SortOrder order);

    void setColumnSortRole(int column, int role);

    void setup();

    SortFilterProxyModel *filterModel();

public slots:
    void clearFilter();

protected slots:
    void setFilterText(QString text);
    void sortChanged(int logicalIndex, Qt::SortOrder);
    void filterTextChanged();
    void lineReturnPressed();

protected:
    QTreeView *m_treeView;
    QStandardItemModel *m_model;
    SortFilterProxyModel *m_filterModel;
    FancyLineEdit *m_lineEdit;

    Qt::ItemDataRole m_role;
    int m_filterColumn;

    int m_defaultRole;
    int m_defaultColumn;
    Qt::SortOrder m_defaultOrder;

    QHash<int, int> m_roles;
};

#endif // MODELVIEWFILTER_H
