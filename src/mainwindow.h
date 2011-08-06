#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>

class IndexDB;
class SettingsDialog;
class BooksViewer;
class BooksListBrowser;
class WelcomeWidget;

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

protected:
    void setupActions();
    void loadSettings();

public slots:
    void aboutAlKotobiya();
    void settingDialog();
    void quranWindow();
    void showBooksList();
    void openBook(int pBookID);
    void lastTabClosed();

private slots:
    void on_actionImport_triggered();
    void on_actionShamelaImport_triggered();

private:
    IndexDB *m_indexDB;
    BooksViewer *m_bookView;
    BooksListBrowser *m_booksList;
    WelcomeWidget *m_welcomeWidget;
    int defaultQuran;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
