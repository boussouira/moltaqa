#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <qdialog.h>

namespace Ui {
    class SettingsDialog;
}

class QSettings;
class QComboBox;
class QFontComboBox;

class FontSelectorDialog : public QDialog
{
    Q_OBJECT

public:

    FontSelectorDialog(QWidget *parent=0);

    void loadFont(const QString &settingKey);

    QString selectedFontFamily();
    int selectedFontSize();

protected slots:
    void saveFont();

protected:
    QComboBox *m_sizeComboBox;
    QFontComboBox *m_fontComboBox;
    QString m_fontSettingKey;
};

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

    static void checkDefaultFonts();

protected:
    void loadSettings();
    void loadStyles();
    void loadSearchFields();
    void loadFontSettings();

    QString getFilePath();
    QString getFolderPath(const QString &defaultPath = QString());

    void saveSetting(QSettings &settings, const QString &group, const QString &key, const QVariant &value, bool needRestart=false);

protected slots:
    void resetSettings();
    void changeBooksDir();
    void saveSettings();
    void deleteSavedSearch();
    void deleteLastOpenedBooks();
    void deleteBooksHistory();
    void optimizeIndex();
    void updateIndex();
    void fontSettingChange();

private slots:
    void on_pushEditRefer_clicked();
    void changeQuranFont();
    void changeDefaultFont();

private:
    Ui::SettingsDialog *ui;
    bool m_needAppRestart;
};

#endif // SETTINGSDIALOG_H
