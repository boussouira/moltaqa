#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <qdialog.h>

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();
    void hideCancelButton(bool hide);

protected:
    void loadSettings();
    void loadStyles();
    QString getFilePath();
    QString getFolderPath(const QString &defaultPath = QString());

protected slots:
    void changeBooksDir();
    void saveSettings();

private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
