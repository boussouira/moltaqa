#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QSettings>

class KSetting;
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
    void checkPaths();

protected slots:
    /// @brief Display information about the application
    /// @todo Improve this message.
    void aboutAlKotobiya();

    /// @brief Show the KSetting dialog.
    void settingDialog();

public slots:
    void quranWindow();
    void showBooksList();
    void openBook(int pBookID);

private:
    BooksViewer *m_bookView;
    BooksListBrowser *m_booksList;
    bool m_createMenu;
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
