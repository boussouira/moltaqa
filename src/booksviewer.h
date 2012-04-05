#ifndef BOOKSVIEWER_H
#define BOOKSVIEWER_H

#include "abstarctview.h"
#include "bookwidgetmanager.h"
#include "clutils.h"
#include "bookpage.h"

class BookWidget;
class LibraryManager;
class TabWidget;
class QMainWindow;
class QToolBar;
class QMenu;
class QComboBox;
class CLuceneQuery;

class BooksViewer : public AbstarctView
{
    Q_OBJECT
public:
    BooksViewer(LibraryManager *libraryManager, QWidget *parent=0);
    ~BooksViewer();
    void setLibraryManager(LibraryManager *libraryManager) { m_libraryManager = libraryManager;}
    QString title();
    void updateToolBars();

    int currentBookID();
    LibraryBookPtr currentBook();
    BookPage *currentPage();

public slots:
    BookWidget *openBook(int bookID, int pageID = -1, CLuceneQuery *query=0);
    void updateActions();
    void showIndexWidget();
    void searchInBook();
    void openTafessir();
    void tabChanged(int newIndex);
    void loadTafessirList();

protected slots:
    void editCurrentBook();

protected:
    void createMenus();

signals:
    void lastTabClosed();

private:
    LibraryManager *m_libraryManager;
    BookWidgetManager *m_viewManager;
    QAction *m_actionEditBook;
    QAction *m_actionNewTab;
    QAction *m_actionIndexDock;
    QAction *m_actionSearchInBook;
    QAction *m_actionNextAYA;
    QAction *m_actionNextPage;
    QAction *m_actionPrevAYA;
    QAction *m_actionPrevPage;
    QAction *m_actionFirstPage;
    QAction *m_actionLastPage;
    QAction *m_actionGotToPage;
    QAction *m_openSelectedTafsir;
    QAction *m_actionCopyPageLink;
    QToolBar *m_toolBarGeneral;
    QToolBar *m_toolBarNavigation;
    QToolBar *m_toolBarTafesir;
    QComboBox *m_comboTafasir;
};

#endif // BOOKSVIEWER_H
