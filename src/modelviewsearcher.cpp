#include "modelviewsearcher.h"
#include "stringutils.h"
#include "utils.h"
#include "modelutils.h"
#include "filterlineedit.h"

#include <qtreeview.h>
#include <qstandarditemmodel.h>

ModelViewSearcher::ModelViewSearcher(QObject *parent) :
    QObject(parent)
{
    m_loopSearch = false;
}

void ModelViewSearcher::setSourceModel(QStandardItemModel *model)
{
    m_model = model;
}

void ModelViewSearcher::setLineEdit(FilterLineEdit *edit)
{
    m_lineEdit = edit;
}

void ModelViewSearcher::setTreeView(QTreeView *view)
{
    m_treeView = view;
}

void ModelViewSearcher::setup()
{
    ml_return_on_fail2(m_treeView, "ModelViewSearcher::setup tree view is not set");
    ml_return_on_fail2(m_model, "ModelViewSearcher::setup model is not set");
    ml_return_on_fail2(m_lineEdit, "ModelViewSearcher::setup line edit is not set");

    if(m_treeView->model() != m_model)
        m_treeView->setModel(m_model);

    connect(m_lineEdit, SIGNAL(delayFilterChanged(QString)), SLOT(setFilterText(QString)));
    connect(m_lineEdit, SIGNAL(returnPressed()), SLOT(lineReturnPressed()));
}

void ModelViewSearcher::clearFilter()
{
    m_lineEdit->clear();
}

bool ModelViewSearcher::search(QStandardItem *item, QString text)
{
    ml_return_val_on_fail(item, false);

    if(!m_skipItems.contains(item)
            && Utils::String::Arabic::clean(item->text()).contains(text)) {
        Utils::Model::selectIndex(m_treeView, m_model->indexFromItem(item));
        m_skipItems.append(item);
        return true;
    }

    if(item->rowCount() > 0) {
        for(int i=0; i<item->rowCount(); i++) {
            if(search(item->child(i), text))
                return true;
        }
    }

    return false;
}

void ModelViewSearcher::setFilterText(QString text)
{
    m_skipItems.clear();

    if(text.trimmed().size())
        m_loopSearch = search(m_model->invisibleRootItem(),
                              Utils::String::Arabic::clean(text.trimmed()));
    else
        m_treeView->collapseAll();
}

void ModelViewSearcher::lineReturnPressed()
{
    bool hasResult = search(m_model->invisibleRootItem(),
                            Utils::String::Arabic::clean(m_lineEdit->text().trimmed()));
    if(m_loopSearch && !hasResult)
        setFilterText(m_lineEdit->text());
}
