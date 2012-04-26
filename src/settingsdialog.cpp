#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <qsettings.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qthread.h>

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
    if(booksPath.size())
        ui->lineBooksDir->setText(booksPath);

    settings.beginGroup("Search");
    ui->spinResultPeerPage->setValue(settings.value("resultPeerPage", 10).toInt());
    ui->spinThreadCount->setValue(settings.value("threadCount", QThread::idealThreadCount()).toInt());
    ui->spinThreadCount->setMinimum(1);
    ui->spinThreadCount->setMaximum(QThread::idealThreadCount()*2);

    ui->comboBox->addItem(tr("%1 ميغا").arg(100), 100);
    ui->comboBox->addItem(tr("%1 ميغا").arg(200), 200);
    ui->comboBox->addItem(tr("%1 ميغا").arg(300), 300);
    ui->comboBox->addItem(tr("%1 ميغا").arg(500), 500);
    ui->comboBox->addItem(tr("%1 جيغا").arg(1), 1000);
    ui->comboBox->addItem(tr("%1 جيغا").arg(1.5), 1500);
    ui->comboBox->addItem(tr("%1 جيغا").arg(2), 2000);
    ui->comboBox->addItem(tr("%1 جيغا").arg(3), 3000);

    int currentSize = settings.value("ramSize", 100).toInt();
    for(int i=0; i<ui->comboBox->count(); i++) {
        if(ui->comboBox->itemData(i).toInt() == currentSize) {
            ui->comboBox->setCurrentIndex(i);
            break;
        }
    }

    settings.endGroup();
}

QString SettingsDialog::getFilePath()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("SQLite database (*.db);;All files (*.*)"));
    dialog.setViewMode(QFileDialog::Detail);

    return (dialog.exec()) ? dialog.selectedFiles().first() : QString();
}

QString SettingsDialog::getFolderPath(const QString &defaultPath)
{
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("اختر مجلد"),
                                                    defaultPath,
                                                    QFileDialog::ShowDirsOnly
                                                    |QFileDialog::DontResolveSymlinks);
    if(dirPath.size()) {
        QDir dir(dirPath);
        return dir.absolutePath();
    }

    return QString();
}

void SettingsDialog::changeBooksDir()
{
    QString filePath = getFolderPath(ui->lineBooksDir->text());
    if(filePath.size()) {
        ui->lineBooksDir->setText(filePath);
    }
}

void SettingsDialog::saveSettings()
{
    QSettings settings;
    QString appPath = ui->lineBooksDir->text();

    settings.setValue("library_dir", appPath);

    settings.beginGroup("Search");
    settings.setValue("resultPeerPage", ui->spinResultPeerPage->value());
    settings.setValue("threadCount", ui->spinThreadCount->value());
    settings.setValue("ramSize", ui->comboBox->itemData(ui->comboBox->currentIndex()));
    settings.endGroup();

    accept();
}

void SettingsDialog::hideCancelButton(bool hide)
{
    ui->pushCancel->setHidden(hide);
}
