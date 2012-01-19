#include "indexwidget.h"
#include "ui_indexwidget.h"
#include "modelenums.h"
#include "bookpage.h"
#include "bookindexmodel.h"

IndexWidget::IndexWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IndexWidget)
{
    ui->setupUi(this);
    sendSignals = true;

    QAction *actionOpenSoraInNewTab = new QAction(tr("فتح في تبويب جديد"), this);

    ui->treeView->addAction(actionOpenSoraInNewTab);
    ui->treeView->setExpandsOnDoubleClick(false);
    ui->treeView->setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(listDoubleClicked(QModelIndex)));
    connect(actionOpenSoraInNewTab, SIGNAL(triggered()),
            this, SLOT(openPageInNewTab()));

}

IndexWidget::~IndexWidget()
{
    delete ui;
}

QModelIndex IndexWidget::findTitle(int tid)
{
    QModelIndex nodeIndex = m_model->index(0, 0, QModelIndex());
    while(nodeIndex.isValid()) {
        int pid = nodeIndex.data(ItemRole::idRole).toInt();

        if(pid == tid) {
            return nodeIndex;
        } else {
            QModelIndex nextIndex = m_model->index(nodeIndex.row()+1, 0, nodeIndex.parent());
            if(!nextIndex.isValid() ||
                    nextIndex.data(ItemRole::idRole).toInt() > tid) {
                QModelIndex index = fitchChild(nodeIndex, tid);
                if(index.isValid())
                    return index;
            }
        }

        nodeIndex = nodeIndex.sibling(nodeIndex.row()+1, 0);
    }

    return QModelIndex();
}

void IndexWidget::setIndex(BookIndexModel *indexModel)
{
    m_model = indexModel;
    ui->treeView->setModel(indexModel);
    ui->treeView->setHeaderHidden(true);
}

void IndexWidget::displayBookInfo()
{
    sendSignals = false;

    ui->spinPage->setValue(qMax(1, m_page->page));
    ui->spinPart->setValue(qMax(1, m_page->part));

    if(m_bookInfo->isQuran()) {
        ui->spinAya->setMaximum(m_page->ayatCount);
        ui->spinAya->setSuffix(QString(" / %1").arg(m_page->ayatCount));
        ui->spinAya->setValue(m_page->aya);
        setSelectedSora(m_page->sora);
    }

    if(m_bookInfo->isTafessir()) {
        ui->spinAya->setValue(m_page->aya);
    }

    selectTitle(m_page->titleID);

    sendSignals = true;

    emit bookInfoChanged();
}

void IndexWidget::setSelectedSora(int pSoraNumber)
{
    QItemSelectionModel *selection = ui->treeView->selectionModel();
    QModelIndex itemToSelect = ui->treeView->model()->index(pSoraNumber - 1, 0, QModelIndex());
    selection->select(itemToSelect,
                      QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    ui->treeView->scrollTo(itemToSelect);
}

void IndexWidget::listDoubleClicked(QModelIndex index)
{
    if(sendSignals) {
        if(m_bookInfo->isQuran()) {
            emit openSora(index.row()+1, 1);
        } else {
            emit openPage(index.data(ItemRole::idRole).toInt());
        }
    }
}

void IndexWidget::openPageInNewTab()
{
    QModelIndex index = ui->treeView->currentIndex();

    if(sendSignals && index.isValid()) {
        if(m_bookInfo->isQuran()) {
            emit openSoraInNewTab(index.row()+1, 1);
        } else {
            emit openPageInNewTab(index.data(ItemRole::idRole).toInt());
        }
    }
}

void IndexWidget::hideAyaSpin(bool visible)
{
    ui->spinAya->setVisible(visible);
    ui->labelAya->setVisible(visible);
}

void IndexWidget::setBookInfo(LibraryBook *book)
{
    m_bookInfo = book;
}

void IndexWidget::setCurrentPage(BookPage *page)
{
    m_page = page;
}

void IndexWidget::selectTitle(int tid)
{
    if(m_model) {
        QModelIndexList selected = ui->treeView->selectionModel()->selectedIndexes();

        // Check if the title is already selected
        if(!selected.isEmpty()) {
            if(selected.at(0).data(ItemRole::idRole).toInt() == tid) {
                return;
            }
        }

        QModelIndex index = findTitle(tid);
        if(index.isValid()) {
            ui->treeView->scrollTo(index);
            ui->treeView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);
        }
    }
}

QTreeView *IndexWidget::treeView()
{
    return ui->treeView;
}

BookIndexModel *IndexWidget::indexModel()
{
    return m_model;
}

QModelIndex IndexWidget::fitchChild(QModelIndex parent, int tid)
{
    QModelIndex nodeIndex = parent.child(0, 0);
    while(nodeIndex.isValid()) {
        int pid = nodeIndex.data(ItemRole::idRole).toInt();

        if(pid == tid) {
            return nodeIndex;
        } else {
            QModelIndex index = fitchChild(nodeIndex, tid);
            if(index.isValid())
                return index;
        }

        nodeIndex = nodeIndex.sibling(nodeIndex.row()+1, 0);
    }

    return QModelIndex();
}
