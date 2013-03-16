#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "searchview.h"
#include "bookslistbrowser.h"

#define MW MainWindow::instance()

class ViewManager;
class LibraryManager;
class SettingsDialog;
class BookReaderView;
class BooksListBrowser;
class WelcomeWidget;
class BookEditorView;
class TarajemRowatView;
class AuthorsView;
class LibraryInfo;
class IndexTracker;
class IndexManager;
class BookReaderHelper;
class QProgressBar;
class LogDialog;
class UpdateChecker;

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
    static MainWindow *instance();

    LibraryInfo *libraryInfo();
    LibraryManager *libraryManager();
    BookReaderView *bookReaderView();
    BooksListBrowser *booksListBrowser();
    IndexTracker *indexTracker();
    IndexManager *indexManager();
    BookReaderHelper *readerHelper();
    SearchView *searchView();
    BookEditorView *editorView();

protected:
    void closeEvent(QCloseEvent *event);
    void setupProgressWidget();
    void setupActions();
    void loadSettings();

public slots:
    void handleMessage(const QString&);

    void stopIndexing();

    void openBook(int bookID, int pageID = -1);

    void showSearchView();
    void showTarajemRowatView();
    void showAuthorsView();

    void searchInFavourites();

    void showBooksList();
    void showBooksList(BooksListBrowser::TreeViewModel model);
    void aboutdDialog();
    void settingDialog();
    void controlCenter();

    void importBookDialog();
    void importFromShamela();

    void exportBooks();

protected slots:
    void indexingStart();
    void indexingStop();
    void indexProgress(int value, int max);
    void showLogDialog();
    void showLibraryInfo();
    void showHelp();
    void checkFinnished();
    void autoUpdateCheck();

    void fullScreenMode();

private:
    Ui::MainWindow *ui;
    ViewManager *m_viewManager;
    LibraryInfo *m_libraryInfo;
    LibraryManager *m_libraryManager;
    BookReaderView *m_bookView;
    BooksListBrowser *m_booksList;
    WelcomeWidget *m_welcomeWidget;
    IndexTracker *m_indexTracker;
    IndexManager *m_indexManager;
    QProgressBar *m_indexBar;
    QWidget *m_progressWidget;
    SearchView *m_searchView;
    BookReaderHelper *m_readerHelper;
    BookEditorView *m_editorView;
    TarajemRowatView *m_tarajemView;
    AuthorsView *m_authorsView;
    LogDialog *m_logDialog;
    UpdateChecker *m_updateChecker;
};

#endif // MAINWINDOW_H
