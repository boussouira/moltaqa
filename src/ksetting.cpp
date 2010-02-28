#include <QSettings>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>

#include "constant.h"
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
        QMessageBox::warning(this, QURAN_DATABASE, WRONG_DATABASE);
    }
}

void KSetting::cancel()
{
   reject();
}
