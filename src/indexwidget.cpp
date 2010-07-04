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
    QAction *actionOpenSora = new QAction(trUtf8("فتح السورة"), ui->treeView);
    QAction *actionOpenSoraInNewTab = new QAction(trUtf8("فتح في تبويب جديد"), this);

    ui->treeView->addAction(actionOpenSora);
    ui->treeView->addAction(actionOpenSoraInNewTab);

    connect(ui->spinBoxAyaNumber, SIGNAL(valueChanged(int)),
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

void IndexWidget::setSoraDetials(PageInfo *pPageInfo)
{
    sendSignals = false;

    ui->spinBoxAyaNumber->setMaximum(pPageInfo->currentSoraAyatCount());
    ui->spinBoxAyaNumber->setSuffix(QString(" / %1").arg(pPageInfo->currentSoraAyatCount()));
    ui->spinBoxAyaNumber->setValue(pPageInfo->currentAya());
    ui->spinBoxPageNumber->setValue(pPageInfo->currentPage());
    setSelectedSora(pPageInfo->currentSoraNumber());

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
    ui->spinBoxPageNumber->setValue(pPageNumber);
    ui->spinBoxAyaNumber->setValue(pAyaNumber);
    sendSignals = true;
}

int IndexWidget::currentPageNmber()
{
    return ui->spinBoxPageNumber->value();
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
    ui->spinBoxAyaNumber->setValue(pAyaNumber);
}
