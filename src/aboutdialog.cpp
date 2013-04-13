#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "CLucene.h"
#include "utils.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    ui->labelVersion->setText(tr("الاصدار: %1").arg(App::version()));

    QString info;
#ifdef GITVERSION
    info += QString("Rev: %1 ").arg(GITVERSION);
#endif

#ifdef DEV_BUILD
    info += " (Developer Build)";
#endif

    info += "<br>";
    info += QString("Qt: %1, ").arg(qVersion());
    info += QString("CLucene: %1").arg(_CL_VERSION);

    ui->labelRevision->setText(info);
}

void AboutDialog::printVersion()
{
    qDebug("Moltaqa library: %s"
#ifdef GITVERSION
           " (%s)"
#endif

#ifdef DEV_BUILD
           " Developer Build"
#endif
           , App::version()
#ifdef GITVERSION
           , GITVERSION
#endif
           );

    qDebug("\t" "Qt: %s", QT_VERSION_STR);
    qDebug("\t" "CLucene: %s", _CL_VERSION);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
