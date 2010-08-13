#include "catslistwidget.h"
#include "bookslistmodel.h"
#include <qtreeview.h>
#include <qboxlayout.h>

CatsListWidget::CatsListWidget(QWidget *parent) :
        QWidget(parent/*, Qt::Dialog*/)
{
    m_treeView = new QTreeView(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_treeView);
    layout->setMargin(0);

    setLayout(layout);
    connect(m_treeView, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(itemDoubleClick(QModelIndex)));
}

void CatsListWidget::setViewModel(QAbstractItemModel *model)
{
    m_treeView->setModel(model);

    m_treeView->hideColumn(1);
    m_treeView->resizeColumnToContents(0);
    m_treeView->setHeaderHidden(true);
}

void CatsListWidget::selectIndex(const QModelIndex &index)
{
    QItemSelectionModel *selection = m_treeView->selectionModel();
    selection->select(index,  QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows);

    m_treeView->scrollTo(index);
}

void CatsListWidget::itemDoubleClick(const QModelIndex &index)
{
    BooksListModel *model =  static_cast<BooksListModel*>(m_treeView->model());
    BooksListNode *node = model->nodeFromIndex(index);

    m_lastSelected = node->getTitle();
    m_lastSelectedID = node->getID();

    emit itemSelected();
}
