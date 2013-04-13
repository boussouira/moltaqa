#ifndef MODELVIEWFILTER_H
#define MODELVIEWFILTER_H

#include <qhash.h>
#include <qobject.h>

class QActionGroup;
class QTreeView;
class SortFilterProxyModel;
class FilterLineEdit;
class QStandardItemModel;
class QAction;
class QMenu;

class FilterInfo
{
public:
    FilterInfo() : column(0), role(Qt::DisplayRole) {}

    int column;
    Qt::ItemDataRole role;
    QString filterName;
};

class ModelViewFilter : public QObject
{
    Q_OBJECT
public:
    ModelViewFilter(QObject *parent = 0);

    void setSourceModel(QStandardItemModel *model);
    void setLineEdit(FilterLineEdit *edit);
    void setTreeView(QTreeView *view);

    void setDefautSortRole(int role);
    void setDefautSortColumn(int column, Qt::SortOrder order);

    void setColumnSortRole(int column, int role);

    void setSorting(int column, int role, Qt::SortOrder order);

    void addFilterColumn(int column, Qt::ItemDataRole role, const QString &filterName);

    void setAllowFilterByDeath(bool allow);

    void setup();

    void reset();

    SortFilterProxyModel *filterModel();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

public slots:
    void clearFilter();

protected slots:
    void setFilterText(QString text);
    void sortChanged(int logicalIndex, Qt::SortOrder);
    void lineReturnPressed();
    void changeFilterAction(QAction *act);

protected:
    QTreeView *m_treeView;
    QStandardItemModel *m_model;
    SortFilterProxyModel *m_filterModel;
    FilterLineEdit *m_lineEdit;

    Qt::ItemDataRole m_role;
    int m_filterColumn;

    int m_defaultRole;
    int m_defaultColumn;
    Qt::SortOrder m_defaultOrder;

    QHash<int, int> m_roles;
    QMenu *m_menu;
    QMenu *m_filterMenu;
    QActionGroup *m_filterActionGroup;

    bool m_allowFilterByDeath;
};

#endif // MODELVIEWFILTER_H
