#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QtSql>

class QStringListModel;
class QuranSearch;
class PageInfo;
class Settings;
class KTab;
class KSetting;
class QuranTextModel;

namespace Ui
{
    class MainWindow;
}

/**
  @brief The application main class.
  */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:

    /**
      @brief Select and display the given SORA number.
      @param pSoraNumber    The SORA number.
      @param pAyaNumber     The AYA number.
      */
    void selectSora(int pSoraNumber, int pAyaNumber = 1);

protected:

    /// @brief Setup the application menus and actions.
    void setupActions();

    /// @brief Connect slots with signals.
    void setupConnections();

    /// @brief Put the Quran SOWAR name in the QListView
    /// @see QuranModel::getSowarList
    void setupQuranIndex();

    /**
     @brief Display the given SORA information.

     Update displayed information such as The current page, AYAT count...
     @param pPageInfo   The current SORA information.
     */
    void setSoraDetials(PageInfo *pPageInfo);

    /**
      @brief Dispay the given PageInfo on the current visible QWebFrame.
      @param pSoranInfo     SORA to display.
      */
    void display(PageInfo *pSoranInfo);

    /// @brief Load setting and do some tests.
    void loadSettings();

    /**
      @brief Make the current frame scroll to the given AYA and SORA number.
      @param pSoraNumber    SORA number.
      @param pAyaNumber     AYA number, the frame well scroll to this given AYA.
      */
    void scrollToAya(int pSoraNumber, int pAyaNumber);

    /**
      @brief Change the selected SORA in the QListView.
      @param pSoraNumber    SORA number which well be selected.
      */
    void setSelectedSora(int pSoraNumber);

    /// @brief Update the navigation *buttons*
    /// by enabling or disabling them after doing some tests.
    void updateNavigationActions();

protected slots:

    /// @brief Hide or show the index dock.
    void hideDockIndex();

    /// @brief Hide or show the search dock.
    void hideDockSearch();

    /**
      @brief This slot is called when the current AYA spin box's value is changed.
      @param pNewAyaNumber      The new AYA number.
      */
    void ayaNumberChange(int pNewAyaNumber);

    /// @brief Open the selected SORA in the QListView.
    void openSelectedSora();

    /// @brief Open the selected SORA in the QListView.
    /// @param pSelection       A QModelIndex.
    void openSelectedSora(QModelIndex pSelection);

    /// @brief Display information about the application
    /// @todo Improve this message.
    void aboutAlKotobiya();

    /// @brief Show the KSetting dialog.
    void settingDialog();

    /// @brief Update the current displayed information.
    /// @see setSoraDetials
    void reloadPageInfo();

    /// @brief Open the selected SORA in a new tab.
    void openSelectedSoraInNewTab();

    /// @brief Add a new tab.
    void addNewTab();

    /// This slot is called when the user press the next AYA button.
    void on_actionNextAYA_triggered();

    /// This slot is called when the user press the previous AYA button.
    void on_actionPrevAYA_triggered();

    /// This slot is called when the user press the next page button.
    void on_actionNextPage_triggered();

    /// This slot is called when the user press the previous page button.
    void on_actionPrevPage_triggered();

private:
    KSetting *m_ksetting;
    QuranTextModel *m_quranModel;
    QuranSearch *m_search;
    Settings *m_settings;
    KTab *m_tab;
    QStringListModel *m_sowarNamesModel;
    QString m_databasePATH;
    bool ignoreSignals;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
