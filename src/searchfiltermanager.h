#ifndef SEARCHFILTERMANAGER_H
#define SEARCHFILTERMANAGER_H

#include "sortfilterproxymodel.h"
#include "filterlineedit.h"
#include "libraryinfo.h"
#include <qstandarditemmodel.h>
#include <qmenu.h>
#include <qsqldatabase.h>
#include <qtreeview.h>

#include <CLucene/StdHeader.h>
#include <CLucene/search/BooleanClause.h>

namespace lucene {
    namespace search {
        class Query;
    }
}

class SearchFilter;

class SearchFilterManager : public QObject
{
    Q_OBJECT

public:
    SearchFilterManager(QObject *parent=0);
    ~SearchFilterManager();

    virtual void loadModel()=0;

    void setSourceModel(QStandardItemModel *model);
    void setTreeView(QTreeView *view);
    void setLineEdit(FilterLineEdit *edit);
    SortFilterProxyModel *filterModel();

    void setChangeFilterColumn(bool changeFilter);
    void setAutoSelectParent(bool autoSelect);

    virtual SearchFilter *getFilterQuery()=0;

    virtual void setSelectedItems(const QList<int> &ids)=0;
    virtual QList<int> getSelectedItems()=0;

protected:
    void setupMenu();

public slots:
    void selectAll();
    void unSelectAll();
    void selectVisible();
    void unSelectVisible();
    void expandFilterView();
    void collapseFilterView();

protected slots:
    void setFilterText(QString text);
    void clearFilter();
    void changeFilterAction(QAction *act);
    void showSelected();
    void showUnSelected();

    void selectChilds();
    void unSelectChilds();

    void showBookInfo();
    void openBook();

    void itemChanged(QStandardItem *item);
    void enableCatSelection();
    void setCatCheckable(QStandardItem *parent, bool checkable);
    void checkChilds(QStandardItem *parent, Qt::CheckState checkStat);
    void checkIndex(QAbstractItemModel *model, const QModelIndex &parent, Qt::CheckState checkStat, bool checkParent);
    bool hasChilds(const QModelIndex &index);

protected:
    SortFilterProxyModel *m_filterModel;
    QStandardItemModel *m_model;
    QTreeView *m_treeView;
    FilterLineEdit *m_lineEdit;
    LibraryInfo *m_libraryInfo;
    QSqlDatabase m_indexDB;
    QMenu *m_menu;
    QAction *m_actFilterByBooks;
    QAction *m_actFilterByAuthors;
    QAction *m_actFilterByAll;
    Qt::ItemDataRole m_role;
    int m_filterColumn;
    bool m_proccessItemChange;
    bool m_autoSelectParent;
    bool m_changeFilterColumn;
};

#endif // SEARCHFILTERMANAGER_H
