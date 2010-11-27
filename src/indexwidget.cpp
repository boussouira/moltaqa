#include "indexwidget.h"
#include "ui_indexwidget.h"
#include "pageinfo.h"
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

void IndexWidget::setIndex(QAbstractItemModel *pList)
{
    ui->treeView->setModel(pList);
    ui->treeView->expandAll();
    ui->treeView->resizeColumnToContents(0);
    ui->treeView->setHeaderHidden(true);
}

void IndexWidget::displayBookInfo()
{
    sendSignals = false;
    int part = qMax(1, m_bookInfo->currentPart());
    ui->spinPage->setMaximum(m_bookInfo->lastPage(part));
    ui->spinPage->setMinimum(m_bookInfo->firstPage(part));
    ui->spinPage->setSuffix(QString(" / %1").arg(m_bookInfo->lastPage(part)));
    ui->spinPage->setValue(qMax(1, m_bookInfo->currentPage()));

    if(m_bookInfo->partsCount() > 1) {
        ui->spinPart->setMaximum(m_bookInfo->partsCount());
        ui->spinPart->setMinimum(1);
        ui->spinPart->setSuffix(QString(" / %1").arg(m_bookInfo->partsCount()));
        ui->spinPart->setValue(part);
    }

    if(m_bookInfo->isQuran()) {
        ui->spinAya->setMaximum(m_bookInfo->currentSoraAyatCount());
        ui->spinAya->setSuffix(QString(" / %1").arg(m_bookInfo->currentSoraAyatCount()));
        ui->spinAya->setValue(m_bookInfo->currentAya());
        setSelectedSora(m_bookInfo->currentSoraNumber());
    }

    sendSignals = true;
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

void IndexWidget::listDoubleClicked(QModelIndex pIndex)
{
    if(sendSignals)
        emit openSora(pIndex.row()+1);
    BookIndexModel *model = static_cast<BookIndexModel*>(ui->treeView->model());
    emit openPage(model->nodeFromIndex(pIndex)->id());
}

void IndexWidget::openSoraInCurrentTab()
{
    emit openSora(ui->treeView->currentIndex().row()+1);
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
