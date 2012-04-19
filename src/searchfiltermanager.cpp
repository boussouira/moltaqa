#include "searchfiltermanager.h"
#include "mainwindow.h"
#include "utils.h"
#include "modelenums.h"
#include "clheader.h"
#include "clconstants.h"
#include "clutils.h"
#include "libraryenums.h"

#include <qsqlquery.h>
#include <qitemselectionmodel.h>
#include <qdebug.h>

SearchFilterManager::SearchFilterManager(QObject *parent)
    : QObject(parent),
      m_model(0),
      m_treeView(0),
      m_lineEdit(0),
      m_menu(0),
      m_role(Qt::DisplayRole),
      m_filterColumn(0),
      m_proccessItemChange(true),
      m_autoSelectParent(true),
      m_changeFilterColumn(true)
{
    m_libraryInfo = MW->libraryInfo();
    m_filterModel = new SortFilterProxyModel(this);

    setupMenu();
}

SearchFilterManager::~SearchFilterManager()
{
    ML_DELETE_CHECK(m_filterModel);
    ML_DELETE_CHECK(m_model);
    ML_DELETE_CHECK(m_menu);
}

void SearchFilterManager::setSourceModel(QStandardItemModel *model)
{
    ML_DELETE_CHECK(m_model);

    m_model = model;
    m_filterModel->setSourceModel(m_model);

    connect(m_model, SIGNAL(itemChanged(QStandardItem*)),
            SLOT(itemChanged(QStandardItem*)));
}

void SearchFilterManager::setTreeView(QTreeView *view)
{
    m_treeView = view;

    if(!m_autoSelectParent) {
        QAction *selectChildAct = new QAction(tr("اختيار العناوين الفرعية"), this);
        QAction *unSelectChildAct = new QAction(tr("الغاء العناوين الفرعية"), this);
        m_treeView->addAction(selectChildAct);
        m_treeView->addAction(unSelectChildAct);

        m_treeView->setContextMenuPolicy(Qt::ActionsContextMenu);

        connect(selectChildAct, SIGNAL(triggered()), SLOT(selectChilds()));
        connect(unSelectChildAct, SIGNAL(triggered()), SLOT(unSelectChilds()));
    }
}

void SearchFilterManager::setLineEdit(FancyLineEdit *edit)
{
    m_lineEdit = edit;
    m_lineEdit->setMenu(m_menu);

    connect(m_lineEdit, SIGNAL(textChanged(QString)),
            SLOT(setFilterText(QString)));
}

void SearchFilterManager::setupMenu()
{
    m_menu = new QMenu(0);

    m_menu->addAction(tr("مسح النص"), this, SLOT(clearFilter()));
    m_menu->addAction(tr("عرض ما تم اختياره"), this, SLOT(showSelected()));
    m_menu->addSeparator();

    if(m_changeFilterColumn) {
        QMenu *menu2 =  m_menu->addMenu(tr("بحث في"));
        QActionGroup *actGroup = new QActionGroup(this);
        m_actFilterByBooks = actGroup->addAction(tr("اسماء الكتب"));
        m_actFilterByBetaka = actGroup->addAction(tr("بطاقة الكتاب"));
        m_actFilterByAuthors = actGroup->addAction(tr("اسماء المؤلفين"));

        m_actFilterByBooks->setCheckable(true);
        m_actFilterByBooks->setChecked(true);
        m_actFilterByBetaka->setCheckable(true);
        m_actFilterByAuthors->setCheckable(true);
        actGroup->setExclusive(true);

        menu2->addActions(actGroup->actions());

        connect(actGroup, SIGNAL(triggered(QAction*)), SLOT(changeFilterAction(QAction*)));
    }

    if(m_lineEdit)
        m_lineEdit->setMenu(m_menu);
}

SortFilterProxyModel *SearchFilterManager::filterModel()
{
    return m_filterModel;
}

void SearchFilterManager::setChangeFilterColumn(bool changeFilter)
{
    if(m_changeFilterColumn != changeFilter) {
        m_changeFilterColumn = changeFilter;

        delete m_menu;
        setupMenu();
    }
}

void SearchFilterManager::setAutoSelectParent(bool autoSelect)
{
    m_autoSelectParent = autoSelect;
}

void SearchFilterManager::setFilterText(QString text)
{
    if(text.size() > 1) {
        text.replace(QRegExp("[\\x0627\\x0622\\x0623\\x0625]"), "[\\x0627\\x0622\\x0623\\x0625]");//ALEFs
        text.replace(QRegExp("[\\x0647\\x0629]"), "[\\x0647\\x0629]"); //TAH_MARBUTA, HEH
        text.replace(QRegExp("[\\x064A\\x0649]"), "[\\x064A\\x0649]"); //YAH, ALEF MAKSOURA

        m_filterModel->setFilterRegExp(text);
        m_treeView->expandAll();

        enableCatSelection();
    } else {
        m_filterModel->setFilterRegExp("");
    }

    m_filterModel->setFilterKeyColumn(m_filterColumn);
    m_filterModel->setFilterRole(m_role);
}

void SearchFilterManager::changeFilterAction(QAction* act)
{
    if(act == m_actFilterByBooks) {
        m_filterColumn = 0;
        m_role = Qt::DisplayRole;
    } else if(act == m_actFilterByAuthors) {
        m_filterColumn = 1;
        m_role = Qt::DisplayRole;
    } else if(act == m_actFilterByBetaka) {
        m_filterColumn = 0;
        m_role = Qt::ToolTipRole;
    }

    m_filterModel->setFilterKeyColumn(m_filterColumn);
    m_filterModel->setFilterRole(m_role);
}

void SearchFilterManager::showSelected()
{
    QVariant checked(Qt::Checked);

    m_filterModel->setFilterKeyColumn(0);
    m_filterModel->setFilterRole(Qt::CheckStateRole);
    m_filterModel->setFilterFixedString(checked.toString());
}

void SearchFilterManager::showUnSelected()
{
    QVariant unChecked(Qt::Unchecked);

    m_filterModel->setFilterRole(Qt::CheckStateRole);
    m_filterModel->setFilterFixedString(unChecked.toString());
}

void SearchFilterManager::selectChilds()
{
    ML_ASSERT(!m_treeView->selectionModel()->selectedIndexes().isEmpty());

    QModelIndex index = m_treeView->selectionModel()->selectedIndexes().first();
    if(index.isValid())
        checkIndex(m_filterModel, index, Qt::Checked, true);
}

void SearchFilterManager::unSelectChilds()
{
    ML_ASSERT(!m_treeView->selectionModel()->selectedIndexes().isEmpty());

    QModelIndex index = m_treeView->selectionModel()->selectedIndexes().first();
    if(index.isValid())
        checkIndex(m_filterModel, index, Qt::Unchecked, true);
}

void SearchFilterManager::clearFilter()
{
    m_lineEdit->clear();
}

void SearchFilterManager::enableCatSelection()
{
    ML_ASSERT(m_autoSelectParent);

    bool catCheckable = m_filterModel->filterRegExp().isEmpty();

    int rowCount = m_filterModel->rowCount();
    QModelIndex topLeft = m_filterModel->index(0, 0);
    QModelIndex bottomRight = m_filterModel->index(rowCount-1, 0);
    QItemSelection selection(topLeft, bottomRight);
    QItemSelection sourceSelection = m_filterModel->mapSelectionToSource(selection);

    foreach (QModelIndex index, sourceSelection.indexes()) {
        QStandardItem *item = m_model->itemFromIndex(index);
        if(item) {
            item->setCheckable(catCheckable);
            setCatCheckable(item, catCheckable);
        }
    }
}

void SearchFilterManager::setCatCheckable(QStandardItem *parent, bool checkable)
{
    int row = 0;
    QStandardItem *child = parent->child(row);

    while(child) {
        if(child->rowCount()) {
            child->setCheckable(checkable);

            setCatCheckable(child, checkable);
        }

        child = parent->child(++row);
    }
}

void SearchFilterManager::selectAll()
{
    int rowCount = m_model->rowCount();
    QModelIndex topLeft = m_model->index(0, 0);
    QModelIndex bottomRight = m_model->index(rowCount-1, 0);
    QItemSelection selection(topLeft, bottomRight);

    m_proccessItemChange = false;

    foreach (QModelIndex index, selection.indexes()) {
        m_model->setData(index, Qt::Checked, Qt::CheckStateRole);
        checkIndex(m_model, index, Qt::Checked, true);
    }

    m_proccessItemChange = true;
}

void SearchFilterManager::unSelectAll()
{
    int rowCount = m_model->rowCount();
    QModelIndex topLeft = m_model->index(0, 0);
    QModelIndex bottomRight = m_model->index(rowCount-1, 0);
    QItemSelection selection(topLeft, bottomRight);

    m_proccessItemChange = false;

    foreach (QModelIndex index, selection.indexes()) {
        m_model->setData(index, Qt::Unchecked, Qt::CheckStateRole);
        checkIndex(m_model, index, Qt::Unchecked, true);
    }

    m_proccessItemChange = true;
}

void SearchFilterManager::selectVisible()
{
    int rowCount = m_filterModel->rowCount();
    QModelIndex topLeft = m_filterModel->index(0, 0);
    QModelIndex bottomRight = m_filterModel->index(rowCount-1, 0);
    QItemSelection selection(topLeft, bottomRight);

    foreach (QModelIndex index, selection.indexes()) {
        if(hasChilds(index)) {
            checkIndex(m_filterModel, index, Qt::Checked, false);
        } else {
            m_filterModel->setData(index, Qt::Checked, Qt::CheckStateRole);
        }
    }
}

void SearchFilterManager::unSelectVisible()
{
    int rowCount = m_filterModel->rowCount();
    QModelIndex topLeft = m_filterModel->index(0, 0);
    QModelIndex bottomRight = m_filterModel->index(rowCount-1, 0);
    QItemSelection selection(topLeft, bottomRight);

    foreach (QModelIndex index, selection.indexes()) {
        if(hasChilds(index)) {
            checkIndex(m_filterModel, index, Qt::Unchecked, false);
        } else {
            m_filterModel->setData(index, Qt::Unchecked, Qt::CheckStateRole);
        }
    }
}

void SearchFilterManager::checkIndex(QAbstractItemModel *model, const QModelIndex &parent, Qt::CheckState checkStat, bool checkParent)
{
    QModelIndex child = parent.child(0, 0);

    while(child.isValid()) {
        if(hasChilds(child)) {
            if(checkParent)
                model->setData(child, checkStat, Qt::CheckStateRole);

            checkIndex(model, child, checkStat, checkParent);
        } else {
            model->setData(child, checkStat, Qt::CheckStateRole);
        }

        child = parent.child(child.row()+1, 0);
    }
}

bool SearchFilterManager::hasChilds(const QModelIndex &index)
{
    return index.child(0, 0).isValid();
}

void SearchFilterManager::expandFilterView()
{
    Q_CHECK_PTR(m_treeView);

    m_treeView->expandAll();
}

void SearchFilterManager::collapseFilterView()
{
    Q_CHECK_PTR(m_treeView);

    m_treeView->collapseAll();
}

void SearchFilterManager::itemChanged(QStandardItem *item)
{
    if(m_autoSelectParent && m_proccessItemChange){
        m_proccessItemChange = false;

        if(item->checkState() != Qt::PartiallyChecked) {
            checkChilds(item, item->checkState());
        }

        QStandardItem *parent = item->parent();

        while(parent) {
            int checkItems = 0;
            int partiallyChecked = 0;

            for(int i=0; i<parent->rowCount(); i++) {
                if(parent->child(i)->checkState()==Qt::Checked)
                    checkItems++;
                else if(parent->child(i)->checkState()==Qt::PartiallyChecked)
                    partiallyChecked++;
            }

            if(checkItems == 0 && partiallyChecked == 0)
                parent->setCheckState(Qt::Unchecked);
            else if(checkItems < parent->rowCount() || partiallyChecked)
                parent->setCheckState(Qt::PartiallyChecked);
            else
                parent->setCheckState(Qt::Checked);

            parent = parent->parent();
        }

        m_proccessItemChange = true;
    }
}

void SearchFilterManager::checkChilds(QStandardItem *parent, Qt::CheckState checkStat)
{
    int row = 0;
    QStandardItem *child = parent->child(0);

    while(child) {
        child->setCheckState(checkStat);
        checkChilds(child, checkStat);

        child = parent->child(++row);
    }
}
