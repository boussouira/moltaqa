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

    connect(ui->pushChangeQuranDBPath, SIGNAL(clicked()), this, SLOT(changeAppDir()));
    connect(ui->pushSaveSettings, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->pushCancel, SIGNAL(clicked()), this, SLOT(cancel()));
}

KSetting::~KSetting()
{
    delete ui;
}

void KSetting::loadSettings()
{
    QSettings settings;
    QString quranDBPath = settings.value("General/app_dir",
                                         QApplication::applicationDirPath()).toString();
    if(!quranDBPath.isEmpty())
        ui->lineAppDir->setText(quranDBPath);
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
void KSetting::changeAppDir()
{
    QString filePath = getFolderPath();
    if(!filePath.isEmpty()) {
        if(filePath.endsWith(QChar('/')) || filePath.endsWith(QChar('\\')))
            filePath.remove(filePath.size()-1, 1);
        QDir appDir(filePath);
        if(appDir.exists("books"))
            ui->lineAppDir->setText(filePath);
        else
            QMessageBox::warning(this,
                                 trUtf8("مجلد البرنامج"),
                                 trUtf8("لقد قمت باختيار مجلد غير صحيح"));
    }
}

void KSetting::saveSettings()
{
    QSettings settings;
    QString quranDBPath = ui->lineAppDir->text();
    if(QFile::exists(quranDBPath)) {
        settings.setValue("General/app_dir", quranDBPath);
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

void KSetting::hideCancelButton(bool hide)
{
    ui->pushCancel->setHidden(hide);
}
