#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>
#include <booksindexdb.h>

class SettingsDialog;
class BooksViewer;
class BooksListBrowser;

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

protected:
    /// @brief Setup the application menus and actions.
    void setupActions();
    void loadSettings();

protected slots:
    /// @brief Display information about the application
    /// @todo Improve this message.
    void aboutAlKotobiya();
    void settingDialog();

public slots:
    void quranWindow();
    void showBooksList();
    void openBook(int pBookID);

private:
    BooksIndexDB m_indexDB;
    BooksViewer *m_bookView;
    BooksListBrowser *m_booksList;
    bool m_createMenu;
    int defaultQuran;
    Ui::MainWindow *ui;


private slots:
    void on_actionImportFromShamela_triggered();
};

#endif // MAINWINDOW_H
