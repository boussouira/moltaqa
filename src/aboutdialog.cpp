#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "utils.h"
#include "CLucene.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    ui->labelVersion->setText(tr("الاصدار: %1").arg(App::version()));

    QString info;
#ifdef GITCHANGENUMBER
    QString("Git: %1 (rev: %2) - ").arg(GITVERSION).arg(GITCHANGENUMBER));
#endif

    info += QString("Qt: %1 - ").arg(QT_VERSION_STR);
    info += QString("CLucene: %1").arg(_CL_VERSION);

    ui->labelRevision->setText(info);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}