#include "edittaffasirlistwidget.h"
#include "ui_edittaffasirlistwidget.h"
#include "modelenums.h"
#include "modelutils.h"
#include "mainwindow.h"
#include "librarymanager.h"
#include "taffesirlistmanager.h"

EditTaffasirListWidget::EditTaffasirListWidget(QWidget *parent) :
    AbstractEditWidget(parent),
    ui(new Ui::EditTaffasirListWidget)
{
    ui->setupUi(this);

    m_model = 0;
    m_taffesirManager = MW->libraryManager()->taffesirListManager();

    loadModel();
    updateActions();

    connect(ui->toolMoveUp, SIGNAL(clicked()), SLOT(moveUp()));
    connect(ui->toolMoveDown, SIGNAL(clicked()), SLOT(moveDown()));
}

EditTaffasirListWidget::~EditTaffasirListWidget()
{
    if(m_model)
        delete m_model;

    delete ui;
}

void EditTaffasirListWidget::loadModel()
{
    m_model = m_taffesirManager->taffesirListModel(true);

    ui->treeView->setModel(m_model);
    ui->treeView->resizeColumnToContents(0);

    connect(ui->treeView->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(updateActions()));
    connect(m_model, SIGNAL(layoutChanged()), SLOT(updateActions()));
}

void EditTaffasirListWidget::save()
{
    m_taffesirManager->save(m_model);
}

void EditTaffasirListWidget::beginEdit()
{
}

void EditTaffasirListWidget::moveUp()
{
    Utils::moveUp(m_model, ui->treeView);
}

void EditTaffasirListWidget::moveDown()
{
    Utils::moveDown(m_model, ui->treeView);
}

void EditTaffasirListWidget::updateActions()
{
    if(!m_model || ui->treeView->selectionModel()->selectedIndexes().isEmpty()) {
        ui->toolMoveDown->setEnabled(false);
        ui->toolMoveUp->setEnabled(false);
    } else {
        QModelIndex index = ui->treeView->selectionModel()->selectedIndexes().first();
        QModelIndex prevIndex = index.sibling(index.row()+1, index.column());
        QModelIndex nextIndex = index.sibling(index.row()-1, index.column());

        ui->toolMoveUp->setEnabled(nextIndex.isValid());
        ui->toolMoveDown->setEnabled(prevIndex.isValid());
    }
}
