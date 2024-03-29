#include "newlibrarydialog.h"
#include "ui_newlibrarydialog.h"
#include "librarycreator.h"
#include "utils.h"

#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qtextstream.h>

NewLibraryDialog::NewLibraryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewLibraryDialog)
{
    ui->setupUi(this);
}

NewLibraryDialog::~NewLibraryDialog()
{
    delete ui;
}

void NewLibraryDialog::on_pushCreate_clicked()
{
    if(ui->lineLibName->text().isEmpty()) {
        QMessageBox::warning(this,
                             tr("انشاء مكتبة جديدة"),
                             tr("لم تقم باختيار اسم المكتبة"));
        return;
    }

    if(ui->lineLibDir->text().isEmpty()) {
        QMessageBox::warning(this,
                             tr("انشاء مكتبة جديدة"),
                             tr("لم تقم باختيار مجلد المكتبة"));
        return;
    }

    createLibrary(ui->lineLibName->text(),
                  ui->lineLibDir->text(),
                  ui->plainTextEdit->toPlainText());
}

void NewLibraryDialog::on_toolSelectLibDir_clicked()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("اختر مجلد"),
                                                    ui->lineLibDir->text(),
                                                    QFileDialog::ShowDirsOnly
                                                    |QFileDialog::DontResolveSymlinks);
    if(dirPath.size()) {
        if(Utils::Library::isValidLibrary(dirPath)) {
            int ret = QMessageBox::question(this,
                                            App::name(),
                                            tr("لقد تم العثور على مكتبة في المجلد الذي اخترته" "<br>"
                                               "هل تريد استخدام هذه المكتبة؟"),
                                            QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
            if(ret == QMessageBox::No) {
                QMessageBox::warning(this,
                                     App::name(),
                                     tr("من فضلك قم باختيار مجلد اخر"));
            } else {
                m_path = dirPath;
                ui->lineLibDir->setText(QDir::toNativeSeparators(dirPath));
                accept();
            }
        } else {
            m_path = dirPath;
            ui->lineLibDir->setText(QDir::toNativeSeparators(dirPath));
        }
    }
}

void NewLibraryDialog::createLibrary(QString name, QString path, QString description)
{
    if(!QFile::exists(path)) {
        qDebug() << "NewLibraryDialog::createLibrary make path:" << path;
        QDir dir;
        dir.mkpath(path);
    }

    QDir libPath(path);
    libPath.mkdir("books");
    libPath.mkdir("data");
    libPath.mkdir("index");
    libPath.mkdir("temp");

    QString infoPath = libPath.filePath("info.xml");

    QFile info(infoPath);
    ml_return_on_fail2(info.open(QIODevice::WriteOnly | QIODevice::Text),
               "createLibrary: open file error:" << info.errorString());

    QTextStream out(&info);
    out.setCodec("utf-8");

    out << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>" << "\n";
    out << "<library-info revision=\"1\">" << "\n";
    out << "    <name>" << name << "</name>" << "\n";
    out << "    <books-dir>" << "books" << "</books-dir>" << "\n";
    out << "    <temps-dir>" << "temp" << "</temps-dir>" << "\n";
    out << "    <description>" << description << "</description>" << "\n";
    out << "</library-info>" << "\n";

    QMessageBox::information(this,
                             tr("انشاء مكتبة جديدة"),
                             tr("تم انشاء المكتبة"));

    accept();
}

QString NewLibraryDialog::libraryDir()
{
    return m_path;
}
