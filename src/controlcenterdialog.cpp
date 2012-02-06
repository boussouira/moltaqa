#include "controlcenterdialog.h"
#include "ui_controlcenterdialog.h"
#include "editcatwidget.h"
#include "editbookslistwidget.h"
#include "edittaffasirlistwidget.h"
#include <qevent.h>

ControlCenterDialog::ControlCenterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ControlCenterDialog)
{
    ui->setupUi(this);

    addEditWidget(0, new EditCatWidget(this));
    addEditWidget(1, new EditBooksListWidget(this));
    addEditWidget(2, new EditTaffasirListWidget(this));

    ui->listWidget->setCurrentRow(0);
    setCurrentRow(0);

    connect(ui->listWidget, SIGNAL(currentRowChanged(int)), SLOT(rowChanged(int)));
}

ControlCenterDialog::~ControlCenterDialog()
{
    delete ui;
}

void ControlCenterDialog::addEditWidget(int index, AbstractEditWidget *w)
{
    ui->stackedWidget->insertWidget(index, w);
    ui->listWidget->insertItem(index, w->windowTitle());
}

void ControlCenterDialog::closeEvent(QCloseEvent *event)
{
    save();
    event->accept();
}

void ControlCenterDialog::rowChanged(int row)
{
    save();

    setCurrentRow(row);
}

void ControlCenterDialog::setCurrentRow(int row)
{
    ui->stackedWidget->setCurrentIndex(row);
    ui->labelControlName->setText(ui->stackedWidget->currentWidget()->windowTitle());

    beginEdit();
}

void ControlCenterDialog::save()
{
    AbstractEditWidget *w = qobject_cast<AbstractEditWidget *>(ui->stackedWidget->currentWidget());

    if(w) {
        w->save();
    }
}

void ControlCenterDialog::beginEdit()
{
    AbstractEditWidget *w = qobject_cast<AbstractEditWidget *>(ui->stackedWidget->currentWidget());

    if(w) {
        w->beginEdit();
    }
}

void ControlCenterDialog::enableSave(bool enable)
{
    Q_UNUSED(enable)
}
