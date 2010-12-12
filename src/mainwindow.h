#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>

class IndexDB;
class SettingsDialog;
class BooksViewer;
class BooksListBrowser;

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

protected slots:
    void aboutAlKotobiya();
    void settingDialog();

public slots:
    void quranWindow();
    void showBooksList();
    void openBook(int pBookID);

private:
    IndexDB *m_indexDB;
    BooksViewer *m_bookView;
    BooksListBrowser *m_booksList;
    bool m_createMenu;
    int defaultQuran;
    Ui::MainWindow *ui;


private slots:
    void on_actionImportFromShamela_triggered();
};

#endif // MAINWINDOW_H
