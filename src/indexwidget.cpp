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
    QAction *actionOpenSora = new QAction(tr("فتح السورة"), ui->treeView);
    QAction *actionOpenSoraInNewTab = new QAction(tr("فتح في تبويب جديد"), this);

    ui->treeView->addAction(actionOpenSora);
    ui->treeView->addAction(actionOpenSoraInNewTab);
    ui->treeView->setExpandsOnDoubleClick(false);

    connect(ui->spinAya, SIGNAL(valueChanged(int)),
            this, SLOT(ayaNumChange(int)));
    connect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(listDoubleClicked(QModelIndex)));
    connect(actionOpenSora, SIGNAL(triggered()),
            this, SLOT(openSoraInCurrentTab()));
    connect(actionOpenSoraInNewTab, SIGNAL(triggered()),
            this, SLOT(openSoraInNewTab()));

}

IndexWidget::~IndexWidget()
{
    delete ui;
}

void IndexWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
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

void IndexWidget::updatePageAndAyaNum(int pPageNumber, int pAyaNumber)
{
    sendSignals = false;
    ui->spinPage->setValue(pPageNumber);
    ui->spinAya->setValue(pAyaNumber);
    sendSignals = true;
}

int IndexWidget::currentPageNmber()
{
    return ui->spinPage->value();
}

void IndexWidget::ayaNumChange(int pAyaNum)
{
    if(sendSignals)
        emit ayaNumberChange(pAyaNum);
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

void IndexWidget::openSoraInCurrentTab()
{
    emit openSora(ui->treeView->currentIndex().row()+1, 1);
}

void IndexWidget::openSoraInNewTab()
{
    emit openSoraInNewTab(ui->treeView->currentIndex().row()+1);
}

void IndexWidget::updateAyaNumber(int pAyaNumber)
{
    ui->spinAya->setValue(pAyaNumber);
}

void IndexWidget::hideAyaSpin(bool visible)
{
    ui->spinAya->setVisible(visible);
    ui->labelAya->setVisible(visible);
}

void IndexWidget::hidePageSpin(bool visible)
{
    ui->spinPage->setVisible(visible);
    ui->labelPage->setVisible(visible);
}

void IndexWidget::hidePartSpin(bool visible)
{
    ui->spinPart->setVisible(visible);
    ui->labelPart->setVisible(visible);
}

void IndexWidget::selectTitle(int tid)
{
    if(!m_model)
        return;

    QModelIndexList selected = ui->treeView->selectionModel()->selectedIndexes();

    if(!selected.isEmpty()) {
        if(selected.at(0).data(ItemRole::idRole).toInt() == tid) {
            return;
        }
    }

    QModelIndex nodeIndex = m_model->index(0, 0, QModelIndex());
    while(nodeIndex.isValid()) {
        int pid = nodeIndex.data(ItemRole::idRole).toInt();

        if(pid == tid) {
            ui->treeView->scrollTo(nodeIndex);
            ui->treeView->selectionModel()->setCurrentIndex(nodeIndex, QItemSelectionModel::SelectCurrent);
            break;
        } else {
            QModelIndex nextIndex = m_model->index(nodeIndex.row()+1, 0, nodeIndex.parent());
            if(!nextIndex.isValid() ||
                    nextIndex.data(ItemRole::idRole).toInt() > tid) {
                if(fitchChild(nodeIndex, tid)) {
                    break;
                }
            }
        }

        nodeIndex = nodeIndex.sibling(nodeIndex.row()+1, 0);
    }
}

QTreeView *IndexWidget::treeView()
{
    return ui->treeView;
}

bool IndexWidget::fitchChild(QModelIndex parent, int tid)
{
    QModelIndex nodeIndex = parent.child(0, 0);
    while(nodeIndex.isValid()) {
        int pid = nodeIndex.data(ItemRole::idRole).toInt();

        if(pid == tid) {
            ui->treeView->scrollTo(nodeIndex);
            ui->treeView->selectionModel()->setCurrentIndex(nodeIndex, QItemSelectionModel::SelectCurrent);
            return true;
        } else {
            if(fitchChild(nodeIndex, tid)) {
                return true;
            }
        }

        nodeIndex = nodeIndex.sibling(nodeIndex.row()+1, 0);
    }

    return false;
}
