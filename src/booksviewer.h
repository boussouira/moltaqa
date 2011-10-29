#ifndef BOOKSVIEWER_H
#define BOOKSVIEWER_H

#include "abstarctview.h"
#include "bookwidgetmanager.h"

class BookWidget;
class LibraryManager;
class TabWidget;
class QMainWindow;
class QToolBar;
class QMenu;
class QComboBox;

class BooksViewer : public AbstarctView
{
    Q_OBJECT
public:
    BooksViewer(LibraryManager *libraryManager, QMainWindow *parent=0);
    ~BooksViewer();
    void setLibraryManager(LibraryManager *libraryManager) { m_libraryManager = libraryManager;}
    QString title();
    void showToolBars();
    void showMenu();
    void hideMenu();

public slots:
    int openBook(int bookID, int pageID = -1);
    void nextUnit();
    void previousUnit();
    void nextPage();
    void previousPage();
    void firstPage();
    void lastPage();
    void goToPage();
    void updateActions();
    void showIndexWidget();
    void openTafessir();
    void openShareeh();
    void tabChanged(int newIndex);

protected:
    void createMenus(QMainWindow *parent);

signals:
    void lastTabClosed();

private:
    LibraryManager *m_libraryManager;
    BookWidgetManager *m_viewManager;
    QAction *m_actionNewTab;
    QAction *m_actionIndexDock;
    QAction *m_actionSearchDock;
    QAction *m_actionNextAYA;
    QAction *m_actionNextPage;
    QAction *m_actionPrevAYA;
    QAction *m_actionPrevPage;
    QAction *m_actionFirstPage;
    QAction *m_actionLastPage;
    QAction *m_actionGotToPage;
    QAction *m_actionOpenShareeh;
    QAction *m_openSelectedTafsir;
    QAction *m_navMenu;
    QToolBar *m_toolBarGeneral;
    QToolBar *m_toolBarNavigation;
    QToolBar *m_toolBarTafesir;
    QToolBar *m_toolBarShorooh;
    QComboBox *m_comboTafasir;
};

#endif // BOOKSVIEWER_H
