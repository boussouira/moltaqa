#include "indexwidget.h"
#include "ui_indexwidget.h"
#include "modelenums.h"
#include "bookpage.h"
#include "modelutils.h"
#include "utils.h"
#include "modelviewfilter.h"

#include <qstandarditemmodel.h>

IndexWidget::IndexWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IndexWidget)
{
    ui->setupUi(this);

    sendSignals = true;
    m_filter = new ModelViewFilter(this);

    ui->treeView->setExpandsOnDoubleClick(false);

    ui->toolSyncTitle->setChecked(Utils::Settings::get("IndexWidget/updateTitle",
                                                       true).toBool());

    if(Utils::Settings::get("Style/singleIndexClick", false).toBool()) {
        connect(ui->treeView, SIGNAL(clicked(QModelIndex)),
                SLOT(listClicked(QModelIndex)));
    } else {
        connect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)),
                SLOT(listClicked(QModelIndex)));
    }

    connect(ui->toolSyncTitle, SIGNAL(toggled(bool)),
            SLOT(updateCurrentTitle(bool)));
}

IndexWidget::~IndexWidget()
{
    Utils::Settings::set("IndexWidget/updateTitle",
                         ui->toolSyncTitle->isChecked());

    delete ui;
}

void IndexWidget::setIndex(QStandardItemModel *indexModel)
{
    m_model = indexModel;
    ui->treeView->setHeaderHidden(true);

    m_filter->setSourceModel(indexModel);
    m_filter->setTreeView(ui->treeView);
    m_filter->setLineEdit(ui->lineEdit);

    m_filter->setup();
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

    if(ui->toolSyncTitle->isChecked())
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

void IndexWidget::updateCurrentTitle(bool checked)
{
    if(checked)
        selectTitle(m_page->titleID);
}

void IndexWidget::listClicked(QModelIndex index)
{
    if(sendSignals) {
        int id = index.data(ItemRole::idRole).toInt();

        if(m_bookInfo->isQuran())
            emit openSora(id, 1);
        else
            emit openPage(id);

    }
}

void IndexWidget::hideAyaSpin(bool visible)
{
    ui->spinAya->setVisible(visible);
    ui->labelAya->setVisible(visible);
}

void IndexWidget::setBookInfo(LibraryBookPtr book)
{
    m_bookInfo = book;
}

void IndexWidget::setCurrentPage(BookPage *page)
{
    m_page = page;
}

void IndexWidget::selectTitle(int tid)
{
    ml_return_on_fail2(m_model, "IndexWidget::selectTitle model is null");

    QModelIndex index;
    QModelIndexList selected = ui->treeView->selectionModel()->selectedIndexes();

    // Check if the title is already selected
    if(selected.size()) {
        if(selected.at(0).data(ItemRole::idRole).toInt() == tid)
            index = selected.at(0);
    }

    if(!index.isValid())
        index = Utils::Model::findModelIndex(ui->treeView->model(), tid);

    if(index.isValid()) {
        ui->treeView->clearSelection();
        ui->treeView->scrollTo(index);
        ui->treeView->selectionModel()->setCurrentIndex(index,
                                                        QItemSelectionModel::Clear |
                                                        QItemSelectionModel::SelectCurrent);
    }
}

QTreeView *IndexWidget::treeView()
{
    return ui->treeView;
}

QStandardItemModel *IndexWidget::indexModel()
{
    return m_model;
}
