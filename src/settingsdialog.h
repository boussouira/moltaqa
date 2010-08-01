#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <qdialog.h>

namespace Ui {
    class SettingsDialog;
}
/**
  @brief This class will handle the edit of the application settings.
  */
class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    /**
      @brief The constructor.
      @param parent The parent widget.
      */
    SettingsDialog(QWidget *parent = 0);

    /// @brief The destructor.
    ~SettingsDialog();
    void hideCancelButton(bool hide);

protected:
    /**
      @brief Load the application settings.
      */
    void loadSettings();

    /**
      @brief Let the user to select an existing file.
      @return The selected file path, or an empty string.
      */
    QString getFilePath();

    QString getFolderPath(const QString &defaultPath = QString(), bool noRoot = false);
protected slots:
    /**
      @brief Select the Quran database (\em SQLite).

      This method help us to force the user to choose an existing database.
      @todo Make a test to see if the selected database is a valid one.
      */
    void changeBooksDir();

    /**
      @brief Close the setting window and save the selected settings.
      */
    void saveSettings();
private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
