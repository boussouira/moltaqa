#include "controlcenterdialog.h"
#include "ui_controlcenterdialog.h"
#include "authorsmanagerwidget.h"
#include "booklistmanagerwidget.h"
#include "favouritesmanagerwidget.h"
#include "librarybookmanagerwidget.h"
#include "taffesirlistmanagerwidget.h"
#include "tarajemrowatmanagerwidget.h"
#include "utils.h"

#include <qevent.h>

ControlCenterDialog::ControlCenterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ControlCenterDialog)
{
    ui->setupUi(this);

    addControlWidget(new LibraryBookManagerWidget(this));
    addControlWidget(new AuthorsManagerWidget(this));
    addControlWidget(new TarajemRowatManagerWidget(this));
    addControlWidget(new BookListManagerWidget(this));
    addControlWidget(new FavouritesManagerWidget(this));
    addControlWidget(new TaffesirListManagerWidget(this));

    ui->listWidget->setCurrentRow(0);
    setCurrentRow(0);

    Utils::Widget::restore(this, "ControlCenterDialog");

    connect(ui->listWidget, SIGNAL(currentRowChanged(int)), SLOT(rowChanged(int)));
}

ControlCenterDialog::~ControlCenterDialog()
{
    delete ui;
}

void ControlCenterDialog::addControlWidget(ControlCenterWidget *w)
{
    ui->stackedWidget->addWidget(w);
    ui->listWidget->addItem(w->title());

    w->loadModel();
}

void ControlCenterDialog::closeEvent(QCloseEvent *event)
{
    save();
    Utils::Widget::save(this, "ControlCenterDialog");

    event->accept();
}

ControlCenterWidget *ControlCenterDialog::currentControlWidget()
{
    return qobject_cast<ControlCenterWidget *>(ui->stackedWidget->currentWidget());
}

void ControlCenterDialog::rowChanged(int row)
{
    save();

    setCurrentRow(row);
}

void ControlCenterDialog::setCurrentRow(int row)
{
    aboutToHide();

    ui->stackedWidget->setCurrentIndex(row);

    ControlCenterWidget *w = currentControlWidget();
    if(w) {
        ui->labelControlName->setText(w->title());
        w->aboutToShow();
    }
}

void ControlCenterDialog::save()
{
    ControlCenterWidget *w = currentControlWidget();
    ml_return_on_fail(w);

    w->save();
}

void ControlCenterDialog::aboutToShow()
{
    ControlCenterWidget *w = currentControlWidget();
    ml_return_on_fail(w);

    w->aboutToShow();
}

void ControlCenterDialog::aboutToHide()
{
    ControlCenterWidget *w = currentControlWidget();
    ml_return_on_fail(w);

    w->aboutToHide();
}

void ControlCenterDialog::enableSave(bool enable)
{
    Q_UNUSED(enable)
}
