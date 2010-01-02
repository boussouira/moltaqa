#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtDebug>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QStringList>
#include <QStringListModel>
#include <QFontDialog>
#include <QSettings>
#include <QFile>
#include <QFileDialog>
#include <QtWebKit>

#include "constant.h"
#include "ktext.h"
#include "quransearch.h"
#include "sorainfo.h"
#include "settings.h"
#include "ktab.h"

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
    void selectSora(int pSoraNumber, int pAyaNumber = 1, bool pDisplay = true);

private:
    void setupActions();
    void setupConnections();
    void setupDataBases();
    void setupQuranIndex();
    void setSoraDetials();
    void display(int pPageNumber , int pSoraNumber, int pAyaNumber = 1);
    void loadSettings();
    void scrollToAya(int pSoraNumber, int pAyaNumber);
    void setSelectedSora(int pSoraNumber);

private slots:
    void hideDockIndex();
    void hideDockSearch();
    void ayaNumberChange(int pNewAyaNumber);
    void openSora(QModelIndex pSelection);
    int getAyaPageNumber(int pSoraNumber, int pAyaNumber);
    void aboutAlKotobiya();
    void selectResult(int pSoraNumber, int pAyaNumber);
    void reloadSoraInfo();
    void openSora();
    void openSoraInNewTab();
    void addNewTab();


private:
    QuranSearch *m_search;
    KText *m_text;
    SoraInfo *m_sora;
    Settings *m_settings;
    KTab *m_tab;
    QSqlDatabase m_db;
    QStringListModel *m_sowarNamesModel;
    QSqlQuery *m_query;
    QString m_databasePATH;
    bool freez;


    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
