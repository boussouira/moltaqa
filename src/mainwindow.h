#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>

class IndexDB;
class SettingsDialog;
class BooksViewer;
class BooksListBrowser;
class WelcomeWidget;
class LibraryInfo;

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

    bool init();
    static MainWindow *mainWindow();

    LibraryInfo *libraryInfo();
    IndexDB *indexDB();
    BooksViewer *booksViewer();
    BooksListBrowser *booksListBrowser();

protected:
    void closeEvent(QCloseEvent *event);
    void setupActions();
    void loadSettings();

public slots:
    void aboutAlKotobiya();
    void settingDialog();
    void quranWindow();
    void showBooksList();
    void openBook(int pBookID);
    void lastTabClosed();
    void controlCenter();

private slots:
    void on_actionImport_triggered();
    void on_actionShamelaImport_triggered();

private:
    Ui::MainWindow *ui;
    LibraryInfo *m_libraryInfo;
    IndexDB *m_indexDB;
    BooksViewer *m_bookView;
    BooksListBrowser *m_booksList;
    WelcomeWidget *m_welcomeWidget;
    int defaultQuran;
};

#endif // MAINWINDOW_H
