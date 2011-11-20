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

    EditCatWidget *catWidget = new EditCatWidget(this);
    addEditWidget(0, catWidget);

    EditBooksListWidget *bookWidget = new EditBooksListWidget(this);
    addEditWidget(1, bookWidget);

    EditTaffasirListWidget *taffessirWidget = new EditTaffasirListWidget(this);
    addEditWidget(2, taffessirWidget);

    rowChanged(0);
    enableSave(false);

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

    connect(w, SIGNAL(edited(bool)), SLOT(enableSave(bool)));
}

void ControlCenterDialog::closeEvent(QCloseEvent *event)
{
    save();
    event->accept();
}

void ControlCenterDialog::rowChanged(int row)
{
    save();

    ui->stackedWidget->setCurrentIndex(row);
    ui->labelControlName->setText(ui->stackedWidget->currentWidget()->windowTitle());

    beginEdit();
    enableSave(false);
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
