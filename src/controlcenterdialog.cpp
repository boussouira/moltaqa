#include "controlcenterdialog.h"
#include "ui_controlcenterdialog.h"
#include "editcatwidget.h"

ControlCenterDialog::ControlCenterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ControlCenterDialog)
{
    ui->setupUi(this);

    EditCatWidget *catWidget = new EditCatWidget(this);
    addEditWidget(0, catWidget);

    rowChanged(0);

    connect(ui->listWidget, SIGNAL(currentRowChanged(int)), SLOT(rowChanged(int)));
}

ControlCenterDialog::~ControlCenterDialog()
{
    delete ui;
}

void ControlCenterDialog::addEditWidget(int index, QWidget *w)
{
    ui->stackedWidget->insertWidget(index, w);
    ui->listWidget->insertItem(index, w->windowTitle());
}

void ControlCenterDialog::rowChanged(int row)
{
    ui->stackedWidget->setCurrentIndex(row);
    ui->labelControlName->setText(ui->stackedWidget->currentWidget()->windowTitle());
}
