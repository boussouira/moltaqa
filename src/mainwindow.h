#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>
#include "searchview.h"

#define MW MainWindow::instance()

class ViewManager;
class LibraryManager;
class SettingsDialog;
class BooksViewer;
class BooksListBrowser;
class WelcomeWidget;
class BookEditorView;
class TarajemRowatView;
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
    static MainWindow *instance();

    LibraryInfo *libraryInfo();
    LibraryManager *libraryManager();
    BooksViewer *booksViewer();
    BooksListBrowser *booksListBrowser();
    IndexTracker *indexTracker();
    IndexManager *indexManager();
    BookReaderHelper *readerHelper();
    SearchView *searchView();
    BookEditorView *editorView();

protected:
    void closeEvent(QCloseEvent *event);
    void setupActions();
    void loadSettings();

public slots:
    void openBook(int pBookID);

    void showSearchView();
    void showTarajemRowatView();

    void showBooksList();
    void aboutdDialog();
    void settingDialog();
    void controlCenter();

    void importBookDialog();
    void importFromShamela();

protected slots:
    void startIndexing();
    void stopIndexing();
    void indexProgress(int value, int max);

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
    TarajemRowatView *m_tarajemView;
};

#endif // MAINWINDOW_H
