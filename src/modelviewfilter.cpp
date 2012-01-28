#include "modelviewfilter.h"
#include <qicon.h>

ModelViewFilter::ModelViewFilter(QObject *parent) :
    QObject(parent),
    m_treeView(0),
    m_model(0),
    m_filterModel(new SortFilterProxyModel(this)),
    m_lineEdit(0),

    m_role(Qt::DisplayRole),
    m_filterColumn(0)
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

void ModelViewFilter::setup()
{
    m_lineEdit->setPixmap(QIcon::fromTheme("edit-clear-locationbar-ltr", QIcon(":/images/clear.png")).pixmap(16, 16));

    m_filterModel->setSourceModel(m_model);
    m_treeView->setModel(m_filterModel);

    connect(m_lineEdit, SIGNAL(textChanged(QString)), SLOT(setFilterText(QString)));
    connect(m_lineEdit, SIGNAL(buttonClicked()), m_lineEdit, SLOT(clear()));
}

SortFilterProxyModel *ModelViewFilter::filterModel()
{
    return m_filterModel;
}

void ModelViewFilter::setFilterText(QString text)
{
    if(text.size() > 1) {
        text.replace(QRegExp("[\\x0627\\x0622\\x0623\\x0625]"), "[\\x0627\\x0622\\x0623\\x0625]");//ALEFs
        text.replace(QRegExp("[\\x0647\\x0629]"), "[\\x0647\\x0629]"); //TAH_MARBUTA, HEH
        text.replace(QRegExp("[\\x064A\\x0649]"), "[\\x064A\\x0649]"); //YAH, ALEF MAKSOURA

        m_filterModel->setFilterRegExp(text);
        m_treeView->expandAll();
    } else {
        m_filterModel->setFilterRegExp("");
    }

    m_filterModel->setFilterKeyColumn(m_filterColumn);
    m_filterModel->setFilterRole(m_role);
}

void ModelViewFilter::clearFilter()
{
    m_filterModel->setFilterFixedString("");
    m_lineEdit->clear();
}
