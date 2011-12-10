#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>
#include "searchview.h"

#define MW MainWindow::mainWindow()

class ViewManager;
class LibraryManager;
class SettingsDialog;
class BooksViewer;
class BooksListBrowser;
class WelcomeWidget;
class BookEditorView;
class LibraryInfo;
class IndexTracker;
class IndexManager;
class BookReaderHelper;
class QProgressBar;

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
    LibraryManager *libraryManager();
    BooksViewer *booksViewer();
    BooksListBrowser *booksListBrowser();
    IndexTracker *indexTracker();
    IndexManager *indexManager();
    BookReaderHelper *readerHelper();
    SearchView *searchView();

protected:
    void closeEvent(QCloseEvent *event);
    void setupActions();
    void loadSettings();

protected slots:
    void aboutApp();
    void settingDialog();
    void quranWindow();
    void showBooksList();
    void showSearchView();
    void openBook(int pBookID);
    void lastTabClosed();
    void controlCenter();
    void startIndexing();
    void stopIndexing();
    void indexProgress(int value, int max);
    void editCurrentBook();

private slots:
    void on_actionImport_triggered();
    void on_actionShamelaImport_triggered();

private:
    Ui::MainWindow *ui;
    ViewManager *m_viewManager;
    LibraryInfo *m_libraryInfo;
    LibraryManager *m_libraryManager;
    BooksViewer *m_bookView;
    BooksListBrowser *m_booksList;
    WelcomeWidget *m_welcomeWidget;
    IndexTracker *m_indexTracker;
    IndexManager *m_indexManager;
    QProgressBar *m_indexBar;
    SearchView *m_searchView;
    BookReaderHelper *m_readerHelper;
    BookEditorView *m_editorView;
    int defaultQuran;
};

#endif // MAINWINDOW_H
