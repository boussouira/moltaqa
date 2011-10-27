#ifndef SEARCHFILTERMANAGER_H
#define SEARCHFILTERMANAGER_H

#include "sortfilterproxymodel.h"
#include "fancylineedit.h"
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

class SearchFilter {
public:
    SearchFilter(): filterQuery(0){}
    lucene::search::Query *filterQuery;
    lucene::search::BooleanClause::Occur clause;
};

class SearchFilterManager : public QObject
{
    Q_OBJECT

public:
    SearchFilterManager(QObject *parent=0);
    ~SearchFilterManager();
    void setTreeView(QTreeView *view);
    void setLineEdit(FancyLineEdit *edit);
    SortFilterProxyModel *filterModel();

    SearchFilter *getFilterQuery();

protected:
    void open();
    void close();
    void setupMenu();
    void loadModel();
    void getBookItems(int catID, QStandardItem *catItem);

    void generateLists();
    void getBooks(QModelIndex index);
    QList<int> selectedBooks();
    QList<int> unSelectedBooks();
    int selectedBooksCount();
    int unSelectBooksCount();

public slots:
    void setFilterText(QString text);
    void changeFilterAction();
    void showSelected();
    void showUnSelected();
    void clearFilter();
    void enableCatSelection();

    void selectAllBooks();
    void unSelectAllBooks();
    void selectVisibleBooks();
    void unSelectVisibleBooks();
    void expandFilterView();
    void collapseFilterView();
    void itemChanged(QStandardItem *item);

protected:
    SortFilterProxyModel *m_filterModel;
    QStandardItemModel *m_model;
    QTreeView *m_treeView;
    FancyLineEdit *m_lineEdit;
    LibraryInfo *m_libraryInfo;
    QSqlDatabase m_indexDB;
    QMenu *m_menu;
    QAction *m_actFilterByBooks;
    QAction *m_actFilterByAuthors;
    QAction *m_actFilterByBetaka;
    QString m_filterText;
    QList<int> m_selectedBooks;
    QList<int> m_unSelectedBooks;
    Qt::ItemDataRole m_role;
    int m_filterColumn;
    bool m_proccessItemChange;
};

#endif // SEARCHFILTERMANAGER_H
