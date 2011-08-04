#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <qsettings.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <qfiledialog.h>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    setModal(true);
    loadSettings();

    connect(ui->pushBooksDir, SIGNAL(clicked()), this, SLOT(changeBooksDir()));
    connect(ui->pushSaveSettings, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->pushCancel, SIGNAL(clicked()), this, SLOT(reject()));
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::loadSettings()
{
    QSettings settings;
    QString booksPath = settings.value("library_dir").toString();
    if(!booksPath.isEmpty())
        ui->lineBooksDir->setText(booksPath);
}

QString SettingsDialog::getFilePath()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("SQLite database (*.db);;All files (*.*)"));
    dialog.setViewMode(QFileDialog::Detail);

    return (dialog.exec()) ? dialog.selectedFiles().first() : QString();
}

QString SettingsDialog::getFolderPath(const QString &defaultPath, bool noRoot)
{
    QDir dir = QFileDialog::getExistingDirectory(this, tr("اختر مجلد"),
                                                    defaultPath,
                                                    QFileDialog::ShowDirsOnly
                                                    |QFileDialog::DontResolveSymlinks);

    if(dir.isRoot() && noRoot){
        return QString();
    } else {
        return dir.absolutePath();
    }
}

void SettingsDialog::changeBooksDir()
{
    QString filePath = getFolderPath(ui->lineBooksDir->text());
    if(!filePath.isEmpty()) {
        ui->lineBooksDir->setText(filePath);
    }
}

void SettingsDialog::saveSettings()
{
    QSettings settings;
    QString appPath = ui->lineBooksDir->text();

    settings.setValue("library_dir", appPath);
    accept();
}

void SettingsDialog::hideCancelButton(bool hide)
{
    ui->pushCancel->setHidden(hide);
}
