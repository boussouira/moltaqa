#ifndef NEWLIBRARYDIALOG_H
#define NEWLIBRARYDIALOG_H

#include <QDialog>

namespace Ui {
    class NewLibraryDialog;
}

class NewLibraryDialog : public QDialog
{
    Q_OBJECT

public:
    NewLibraryDialog(QWidget *parent = 0);
    ~NewLibraryDialog();

    void createLibrary(QString name, QString path, QString description);
    QString libraryDir();

private slots:
    void on_pushCreate_clicked();
    void on_toolSelectLibDir_clicked();

private:
    QString m_name;
    QString m_path;
    QString m_desciption;
    Ui::NewLibraryDialog *ui;
};

#endif // NEWLIBRARYDIALOG_H
