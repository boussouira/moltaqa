#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <qdialog.h>

namespace Ui {
    class SettingsDialog;
}

class QSettings;

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();
    void hideCancelButton(bool hide);

protected:
    void loadSettings();
    void loadStyles();
    void loadSearchFields();

    QString getFilePath();
    QString getFolderPath(const QString &defaultPath = QString());

    void saveSetting(QSettings &settings, const QString &group, const QString &key, const QVariant &value, bool needRestart=false);

protected slots:
    void changeBooksDir();
    void saveSettings();
    void deleteSavedSearch();
    void deleteLastOpenedBooks();
    void deleteBooksHistory();
    void optimizeIndex();

private slots:
    void on_pushEditRefer_clicked();

private:
    Ui::SettingsDialog *ui;
    bool m_needAppRestart;
};

#endif // SETTINGSDIALOG_H
