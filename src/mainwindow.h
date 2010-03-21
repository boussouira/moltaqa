#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class KSetting;
class BooksViewer;

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

protected slots:
    /// @brief Display information about the application
    /// @todo Improve this message.
    void aboutAlKotobiya();

    /// @brief Show the KSetting dialog.
    void settingDialog();
    void updateNavigationActions();

private:
    KSetting *m_ksetting;
    BooksViewer *m_booksViewr;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
