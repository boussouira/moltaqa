#include <QSettings>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    setModal(true);
    loadSettings();

    connect(ui->pushChangeQuranDBPath, SIGNAL(clicked()), this, SLOT(changeAppDir()));
    connect(ui->pushSaveSettings, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->pushCancel, SIGNAL(clicked()), this, SLOT(cancel()));
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::loadSettings()
{
    QSettings settings;
    QString appPath = settings.value("General/app_dir",
                                         QApplication::applicationDirPath()).toString();
    if(!appPath.isEmpty())
        ui->lineAppDir->setText(appPath);
}

QString SettingsDialog::getFilePath()
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

QString SettingsDialog::getFolderPath()
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
void SettingsDialog::changeAppDir()
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

void SettingsDialog::saveSettings()
{
    QSettings settings;
    QString appPath = ui->lineAppDir->text();
    if(QFile::exists(appPath)) {
        settings.setValue("General/app_dir", appPath);
        accept();
    } else {
        QMessageBox::warning(this,
                             trUtf8("قاعدة البيانات"),
                             trUtf8("قاعدة البيانات التي قمت بتحديدها غير موجود"));
    }
}

void SettingsDialog::cancel()
{
   reject();
}

void SettingsDialog::hideCancelButton(bool hide)
{
    ui->pushCancel->setHidden(hide);
}
