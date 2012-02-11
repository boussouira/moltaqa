#include "controlcenterdialog.h"
#include "ui_controlcenterdialog.h"
#include "booklistmanagerwidget.h"
#include "librarybookmanagerwidget.h"
#include "taffesirlistmanagerwidget.h"
#include <qevent.h>

ControlCenterDialog::ControlCenterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ControlCenterDialog)
{
    ui->setupUi(this);

    addControlWidget(0, new BookListManagerWidget(this));
    addControlWidget(1, new LibraryBookManagerWidget(this));
    addControlWidget(2, new TaffesirListManagerWidget(this));

    ui->listWidget->setCurrentRow(0);
    setCurrentRow(0);

    connect(ui->listWidget, SIGNAL(currentRowChanged(int)), SLOT(rowChanged(int)));
}

ControlCenterDialog::~ControlCenterDialog()
{
    delete ui;
}

void ControlCenterDialog::addControlWidget(int index, ControlCenterWidget *w)
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
    ControlCenterWidget *w = qobject_cast<ControlCenterWidget *>(ui->stackedWidget->currentWidget());

    if(w) {
        w->save();
    }
}

void ControlCenterDialog::beginEdit()
{
    ControlCenterWidget *w = qobject_cast<ControlCenterWidget *>(ui->stackedWidget->currentWidget());

    if(w) {
        w->beginEdit();
    }
}

void ControlCenterDialog::enableSave(bool enable)
{
    Q_UNUSED(enable)
}
