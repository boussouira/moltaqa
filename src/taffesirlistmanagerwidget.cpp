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
    m_treeManager = new TreeViewEditor(this);
}

TaffesirListManagerWidget::~TaffesirListManagerWidget()
{
    ml_delete_check(m_model);
    ml_delete_check(m_treeManager);

    delete ui;
}

QString TaffesirListManagerWidget::title()
{
    return tr("التفاسير");
}

void TaffesirListManagerWidget::loadModel()
{
    m_model = m_taffesirManager->allTaffesirModel();
    ml_return_on_fail(m_model);

    ui->treeView->setModel(m_model);
    ui->treeView->resizeColumnToContents(0);

    m_treeManager->setMoveUpButton(ui->toolMoveUp);
    m_treeManager->setMoveDownButton(ui->toolMoveDown);

    m_treeManager->setTreeView(ui->treeView);
    m_treeManager->setModel(m_model);
    m_treeManager->setup();
}

void TaffesirListManagerWidget::save()
{
    m_taffesirManager->save(m_model);
}
