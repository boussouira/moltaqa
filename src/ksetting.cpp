#include <QSettings>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>

#include "ksetting.h"
#include "ui_ksetting.h"

KSetting::KSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KSetting)
{
    ui->setupUi(this);
    setModal(true);
    loadSettings();
    connect(ui->pushChangeQuranDBPath, SIGNAL(clicked()), this, SLOT(changeQuranDBPath()));
    connect(ui->pushSaveSettings, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->pushCancel, SIGNAL(clicked()), this, SLOT(cancel()));
}

KSetting::~KSetting()
{
    delete ui;
}

void KSetting::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void KSetting::loadSettings()
{
    QSettings settings;
    QString quranDBPath = settings.value("app/db").toString();
    if(!quranDBPath.isEmpty())
        ui->lineQuranDBPath->setText(quranDBPath);
}

QString KSetting::getFilePath()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("SQLite database (*.db);;All files (*.*)"));
    dialog.setViewMode(QFileDialog::Detail);

    if (dialog.exec())
        return dialog.selectedFiles().first();
    else
        return QString();
}

QString KSetting::getFolderPath()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setOptions(QFileDialog::ShowDirsOnly
                     | QFileDialog::DontResolveSymlinks);

    if (dialog.exec())
        return dialog.selectedFiles().first();
    else
        return QString();
}
void KSetting::changeQuranDBPath()
{
    QString filePath = getFilePath();
    if(!filePath.isEmpty())
        ui->lineQuranDBPath->setText(filePath);
}

void KSetting::saveSettings()
{
    QSettings settings;
    QString quranDBPath = ui->lineQuranDBPath->text();
    if(QFile::exists(quranDBPath)) {
        settings.setValue("app/db", quranDBPath);
        accept();
    } else {
        QMessageBox::warning(this,
                             trUtf8("قاعدة البيانات"),
                             trUtf8("قاعدة البيانات التي قمت بتحديدها غير موجود"));
    }
}

void KSetting::cancel()
{
   reject();
}
