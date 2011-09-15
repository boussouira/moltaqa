#ifndef BOOKSVIEWER_H
#define BOOKSVIEWER_H

#include <qwidget.h>

class BookWidget;
class LibraryManager;
class TabWidget;
class QMainWindow;
class QToolBar;
class QMenu;
class QComboBox;

class BooksViewer : public QWidget
{
    Q_OBJECT
public:
    BooksViewer(LibraryManager *libraryManager, QMainWindow *parent);
    ~BooksViewer();
    void setLibraryManager(LibraryManager *libraryManager) { m_libraryManager = libraryManager;}

public slots:
    void openBook(int bookID, int pageID = -1, bool newTab = true);
    void nextUnit();
    void previousUnit();
    void nextPage();
    void previousPage();
    void firstPage();
    void lastPage();
    void goToPage();
    void updateActions();
    void showIndexWidget();
    void tabChangePosition(int fromIndex, int toIndex);
    void tabCloseRequest(int tabIndex);
    void showToolBar();
    void removeToolBar();
    void openTafessir(); // TODO: should be private?
    void openShareeh();
    BookWidget *currentBookWidget();
    BookWidget *currentBookWidget(int index);
    void tabChanged(int newIndex);

protected:
    void createMenus(QMainWindow *parent);

signals:
    void lastTabClosed();

private:
    TabWidget *m_tab;
    LibraryManager *m_libraryManager;
    QList<BookWidget *> m_bookWidgets;
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
