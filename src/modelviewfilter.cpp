#include "modelviewfilter.h"
#include <qicon.h>
#include <qheaderview.h>

ModelViewFilter::ModelViewFilter(QObject *parent) :
    QObject(parent),
    m_treeView(0),
    m_model(0),
    m_filterModel(new SortFilterProxyModel(this)),
    m_lineEdit(0),

    m_role(Qt::DisplayRole),
    m_filterColumn(0),
    m_defaultRole(-1),
    m_defaultColumn(-1),
    m_defaultOrder(Qt::AscendingOrder)
{
}

void ModelViewFilter::setSourceModel(QStandardItemModel *model)
{
    m_model = model;
}

void ModelViewFilter::setLineEdit(FancyLineEdit *edit)
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

void ModelViewFilter::setup()
{
    m_lineEdit->setPixmap(QIcon::fromTheme("edit-clear-locationbar-ltr", QIcon(":/images/clear.png")).pixmap(16, 16));

    if(m_defaultRole != -1)
        m_filterModel->setSortRole(m_defaultRole);

    m_filterModel->setSourceModel(m_model);

    m_treeView->setModel(m_filterModel);

    if(m_defaultColumn != -1)
        m_treeView->sortByColumn(m_defaultColumn, m_defaultOrder);

    connect(m_lineEdit, SIGNAL(textChanged(QString)), SLOT(filterTextChanged()));
    connect(m_lineEdit, SIGNAL(returnPressed()), SLOT(lineReturnPressed()));
    connect(m_lineEdit, SIGNAL(buttonClicked()), m_lineEdit, SLOT(clear()));
    connect(m_treeView->header(),
            SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),
            SLOT(sortChanged(int,Qt::SortOrder)));
}

SortFilterProxyModel *ModelViewFilter::filterModel()
{
    return m_filterModel;
}

void ModelViewFilter::setFilterText(QString text)
{
    text.replace(QRegExp("[\\x0627\\x0622\\x0623\\x0625]"), "[\\x0627\\x0622\\x0623\\x0625]");//ALEFs
    text.replace(QRegExp("[\\x0647\\x0629]"), "[\\x0647\\x0629]"); //TAH_MARBUTA, HEH
    text.replace(QRegExp("[\\x064A\\x0649]"), "[\\x064A\\x0649]"); //YAH, ALEF MAKSOURA

    m_filterModel->setFilterRegExp(text);
    m_filterModel->setFilterKeyColumn(m_filterColumn);
    m_filterModel->setFilterRole(m_role);

    m_treeView->expandAll();
}

void ModelViewFilter::filterTextChanged()
{
    QString text = m_lineEdit->text().trimmed();

    if(text.size() > 3)
        setFilterText(text);
    else
        m_filterModel->setFilterRegExp("");
}

void ModelViewFilter::lineReturnPressed()
{
    QString text = m_lineEdit->text().trimmed();

    setFilterText(text);
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
