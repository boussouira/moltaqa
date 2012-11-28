#include "modelviewfilter.h"
#include "stringutils.h"
#include "utils.h"

#include <qicon.h>
#include <qheaderview.h>
#include <qevent.h>
#include <qaction.h>
#include <qmenu.h>

Q_DECLARE_METATYPE(FilterInfo)

ModelViewFilter::ModelViewFilter(QObject *parent) :
    QObject(parent),
    m_filterModel(0)
{
    reset();
}

void ModelViewFilter::setSourceModel(QStandardItemModel *model)
{
    m_model = model;
}

void ModelViewFilter::setLineEdit(FilterLineEdit *edit)
{
    m_lineEdit = edit;
}

void ModelViewFilter::setTreeView(QTreeView *view)
{
    m_treeView = view;
}

void ModelViewFilter::setDefautSortRole(int role)
{
    m_defaultRole = role;
}

void ModelViewFilter::setDefautSortColumn(int column, Qt::SortOrder order)
{
    m_defaultColumn = column;
    m_defaultOrder = order;
}

void ModelViewFilter::setColumnSortRole(int column, int role)
{
    m_roles[column] = role;
}

void ModelViewFilter::addFilterColumn(int column, Qt::ItemDataRole role, const QString &filterName)
{
    FilterInfo info;
    info.column = column;
    info.role = role;
    info.filterName = filterName;

    QAction *act = new QAction(filterName, this);
    act->setData(QVariant::fromValue(info));
    act->setCheckable(true);

    if(!m_filterActionGroup || !m_menu || !m_filterMenu) {
        m_menu = new QMenu(0);

        m_filterMenu =  m_menu->addMenu(tr("بحث في"));
        m_filterActionGroup = new QActionGroup(this);
        QAction * actFilterByAll = m_filterActionGroup->addAction(tr("الكل"));
        FilterInfo infoAll;
        infoAll.column = -1;
        infoAll.role = role;
        infoAll.filterName = filterName;
        actFilterByAll->setData(QVariant::fromValue(infoAll));

        m_filterActionGroup->addAction("")->setSeparator(true);

        actFilterByAll->setCheckable(true);
        actFilterByAll->setChecked(true);
        m_filterActionGroup->setExclusive(true);

        connect(m_filterActionGroup, SIGNAL(triggered(QAction*)), SLOT(changeFilterAction(QAction*)));
    }

    m_filterActionGroup->addAction(act);
    m_filterMenu->addActions(m_filterActionGroup->actions());
}

void ModelViewFilter::setAllowFilterByDeath(bool allow)
{
    m_allowFilterByDeath = allow;
}

void ModelViewFilter::setup()
{
    ml_return_on_fail2(m_treeView, "ModelViewFilter::setup tree view is not set");
    ml_return_on_fail2(m_model, "ModelViewFilter::setup model is not set");
    ml_return_on_fail2(m_lineEdit, "ModelViewFilter::setup line edit is not set");

    if(m_defaultRole != -1)
        m_filterModel->setSortRole(m_defaultRole);

    m_filterModel->setSourceModel(m_model);

    m_treeView->setModel(m_filterModel);

    if(m_defaultColumn != -1)
        m_treeView->sortByColumn(m_defaultColumn, m_defaultOrder);

    if(m_menu)
        m_lineEdit->setFilterMenu(m_menu);

    connect(m_lineEdit, SIGNAL(delayFilterChanged(QString)), SLOT(setFilterText(QString)));
    connect(m_lineEdit, SIGNAL(returnPressed()), SLOT(lineReturnPressed()));
    connect(m_treeView->header(),
            SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),
            SLOT(sortChanged(int,Qt::SortOrder)));

    m_treeView->installEventFilter(this);
}

void ModelViewFilter::reset()
{
    m_treeView = 0;
    m_model = 0;
    m_lineEdit = 0;

    m_role = Qt::DisplayRole;
    m_filterColumn = 0;
    m_defaultRole = -1;
    m_defaultColumn = -1;
    m_defaultOrder = Qt::AscendingOrder;
    m_menu = 0;
    m_filterMenu = 0;
    m_filterActionGroup = 0;

    m_allowFilterByDeath = false;

    ml_delete_check(m_filterModel);
    m_filterModel = new SortFilterProxyModel(this);
}

SortFilterProxyModel *ModelViewFilter::filterModel()
{
    return m_filterModel;
}

bool ModelViewFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_treeView && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if((keyEvent->modifiers() & Qt::ControlModifier)) {
            if(keyEvent->key() == Qt::Key_Plus) {
                m_treeView->expandAll();
                return true;
            } else if(keyEvent->key() == Qt::Key_Minus) {
                m_treeView->collapseAll();
                return true;
            }
        }
    }

    return false;
}

void ModelViewFilter::setFilterText(QString text)
{
    if(m_allowFilterByDeath
            && text.contains(QRegExp("[0-9\\?\\*-]+:[0-9\\?\\*-]+"))) {
        QStringList t = text.split(':');
        int dStart = (t.first() == "*") ? 0x80000000 : t.first().toInt();
        int dEnd = (t.last() == "*") ? 99999 : t.last().toInt();

        m_filterModel->setFilterByDeath(dStart, dEnd);
    } else {
        m_filterModel->setArabicFilterRegexp(text);
    }

    if(m_filterModel->filterKeyColumn() != m_filterColumn)
        m_filterModel->setFilterKeyColumn(m_filterColumn);

    if(m_filterModel->filterRole() != m_role)
        m_filterModel->setFilterRole(m_role);

    m_treeView->expandAll();
}

void ModelViewFilter::lineReturnPressed()
{
    QString text = m_lineEdit->text().trimmed();

    setFilterText(text);
}

void ModelViewFilter::changeFilterAction(QAction *act)
{
    ml_return_on_fail(act);
    ml_return_on_fail(act->data().canConvert<FilterInfo>());

    FilterInfo info = act->data().value<FilterInfo>();
    ml_return_on_fail(m_filterColumn != info.column);

    m_filterColumn = info.column;
    m_role = info.role;

    m_filterModel->setFilterKeyColumn(m_filterColumn);
    m_filterModel->setFilterRole(m_role);

    if(!m_filterModel->filterRegExp().isEmpty())
        m_treeView->expandAll();
}

void ModelViewFilter::clearFilter()
{
    m_filterModel->setFilterFixedString("");
    m_lineEdit->clear();
}

void ModelViewFilter::sortChanged(int logicalIndex, Qt::SortOrder)
{
    if(m_roles.contains(logicalIndex))
        m_filterModel->setSortRole(m_roles[logicalIndex]);
    else
        m_filterModel->setSortRole(Qt::DisplayRole);
}
