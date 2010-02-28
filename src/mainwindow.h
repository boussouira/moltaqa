#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>

#include "ui_mainwindow.h"

class QStringListModel;
class QuranSearch;
class SoraInfo;
class Settings;
class KTab;
class QuranModel;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void selectSora(int pSoraNumber, int pAyaNumber = 1, bool pDisplay = true);

private:
    void setupActions();
    void setupConnections();
    void setupQuranIndex();
    void setSoraDetials(SoraInfo *pSoraInfo);
    void display(SoraInfo *pSoranInfo);
    void loadSettings();
    void saveSettings();
    void scrollToAya(int pSoraNumber, int pAyaNumber);
    void setSelectedSora(int pSoraNumber);

private slots:
    void hideDockIndex();
    void hideDockSearch();
    void ayaNumberChange(int pNewAyaNumber);
    void openSelectedSora();
    void openSelectedSora(QModelIndex pSelection);
    void aboutAlKotobiya();
    void reloadSoraInfo();
    void openSelectedSoraInNewTab();
    void addNewTab();

private:
    QuranModel *m_quranModel;
    QuranSearch *m_search;
    SoraInfo *m_sora;
    Settings *m_settings;
    KTab *m_tab;
    QStringListModel *m_sowarNamesModel;
    QString m_databasePATH;
    bool freez;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
