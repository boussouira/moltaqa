#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
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

#include "constant.h"
#include "ctextcrusor.h"
#include "quransearch.h"

#define DATABASEPATH "Quran.db"

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
    void selectSora(int psoraNumber, int payaNumber = 1, bool pDisplay = true);

private:
    void setSoraDetials();
    void display(int pPageNumber , int pSoraNumber, int pAyaNumber = 1);
    void LoadSettings();
    void scrollToAya(int pSoraNumber, int pAyaNumber);
    void setSelectedSora(int pSoraNumber);

private slots:
    void pageNumberChange(int pNewPageNumbe);
    void ayaNumberChange(int pNewAyaNumber);
    void selectedSoraChange(QModelIndex pselection);
    int getAyaPageNumber(int pSoraNumber, int pAyaNumber);
    void getFirsLastAyaNumberInPage(int pSoraNumber, int pPageNumber, int *pFirstAya, int *pLastAya);
    void getFirsLastSoraNumberInPage(int pPageNumber, int *pFirstSoraNumber, int *pLastSoraNumber);
    void textChangeFont();
    void aboutQR();
    void selectResult(int pSoraNumber, int pAyaNumber);

private:
    QuranSearch *m_search;
    CTextCrusor *m_textCrusor;
    QSqlDatabase m_db;
    QStringListModel *m_sowarNamesModel;
    QSqlQuery *m_query;
    QSettings *settings;
    QString m_databasePATH;
    QString m_currentSoraName ;
    QString m_currentSoraDescent;
    int m_currentSoraNumber;
    int m_currentAyaNumber;
    int m_currentSoraAyatCount ;
    int m_currentPageNumber;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
