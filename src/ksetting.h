#ifndef KSETTING_H
#define KSETTING_H

#include <QWidget>
#include <QDialog>

namespace Ui {
    class KSetting;
}
/**
  @brief This class will handle the edit of the application settings.
  */
class KSetting : public QDialog {
    Q_OBJECT
public:
    /**
      @brief The constructor.
      @param parent The parent widget.
      */
    explicit KSetting(QWidget *parent = 0);

    /// @brief The destructor.
    ~KSetting();
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

    QString getFolderPath();
protected slots:
    /**
      @brief Select the Quran database (\em SQLite).

      This method help us to force the user to choose an existing database.
      @todo Make a test to see if the selected database is a valid one.
      */
    void changeAppDir();

    /**
      @brief Close the setting window without saving any thing.
      */
    void cancel();

    /**
      @brief Close the setting window and save the selected settings.
      */
    void saveSettings();
private:
    Ui::KSetting *ui;
};

#endif // KSETTING_H
