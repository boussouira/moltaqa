#include "taffesirlistmanagerwidget.h"
#include "ui_taffesirlistmanagerwidget.h"
#include "modelenums.h"
#include "modelutils.h"
#include "librarymanager.h"
#include "taffesirlistmanager.h"
#include "utils.h"

TaffesirListManagerWidget::TaffesirListManagerWidget(QWidget *parent) :
    ControlCenterWidget(parent),
    ui(new Ui::TaffesirListManagerWidget)
{
    ui->setupUi(this);

    m_model = 0;
    m_taffesirManager = LibraryManager::instance()->taffesirListManager();

    updateActions();

    connect(ui->toolMoveUp, SIGNAL(clicked()), SLOT(moveUp()));
    connect(ui->toolMoveDown, SIGNAL(clicked()), SLOT(moveDown()));
}

TaffesirListManagerWidget::~TaffesirListManagerWidget()
{
    ML_DELETE_CHECK(m_model);

    delete ui;
}

QString TaffesirListManagerWidget::title()
{
    return tr("التفاسير");
}

void TaffesirListManagerWidget::loadModel()
{
    m_model = m_taffesirManager->allTaffesirModel();

    ui->treeView->setModel(m_model);
    ui->treeView->resizeColumnToContents(0);

    connect(ui->treeView->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(updateActions()));
    connect(m_model, SIGNAL(layoutChanged()), SLOT(updateActions()));
}

void TaffesirListManagerWidget::save()
{
    m_taffesirManager->save(m_model);
}

void TaffesirListManagerWidget::moveUp()
{
    Utils::Model::moveUp(m_model, ui->treeView);
}

void TaffesirListManagerWidget::moveDown()
{
    Utils::Model::moveDown(m_model, ui->treeView);
}

void TaffesirListManagerWidget::updateActions()
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
