#include "modelviewfilter.h"
#include "stringutils.h"
#include "utils.h"

#include <qicon.h>
#include <qheaderview.h>
#include <qevent.h>

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

    connect(m_lineEdit, SIGNAL(textChanged(QString)), SLOT(filterTextChanged()));
    connect(m_lineEdit, SIGNAL(returnPressed()), SLOT(lineReturnPressed()));
    connect(m_treeView->header(),
            SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),
            SLOT(sortChanged(int,Qt::SortOrder)));

    m_treeView->installEventFilter(this);
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
    m_filterModel->setArabicFilterRegexp(text);
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
