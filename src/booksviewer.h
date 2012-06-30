#ifndef BOOKSVIEWER_H
#define BOOKSVIEWER_H

#include "abstarctview.h"
#include "bookwidgetmanager.h"
#include "clutils.h"
#include "bookpage.h"

class BookWidget;
class LibraryManager;
class LibraryBookManager;
class TaffesirListManager;
class TabWidget;
class QMainWindow;
class QToolBar;
class QMenu;
class QComboBox;
class CLuceneQuery;
class FilterLineEdit;

class BooksViewer : public AbstarctView
{
    Q_OBJECT
public:
    BooksViewer(LibraryManager *libraryManager, QWidget *parent=0);
    ~BooksViewer();

    void setLibraryManager(LibraryManager *libraryManager) { m_libraryManager = libraryManager;}
    QString title();
    QString viewLink();
    void updateToolBars();

    int currentBookID();
    LibraryBookPtr currentBook();
    BookPage *currentPage();

    BookWidgetManager *bookWidgetManager() { return m_viewManager; }

public slots:
    BookWidget *openBook(int bookID, int pageID = -1, CLuceneQuery *query=0);
    void updateActions();
    void showIndexWidget();
    void searchInBook();
    void openTafessir();
    void tabChanged(int newIndex);
    void loadTafessirList();
    void searchInPage();
    void searchNext();
    void searchPrev();

protected slots:
    void editCurrentBook();

protected:
    void createMenus();
    void updateSearchNavigation();

signals:
    void lastTabClosed();

private:
    LibraryManager *m_libraryManager;
    LibraryBookManager *m_bookManager;
    TaffesirListManager *m_taffesirManager;
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
    QAction *m_bookInfoAct;
    QAction *m_openSelectedTafsir;
    QToolBar *m_toolBarGeneral;
    QToolBar *m_toolBarNavigation;
    QToolBar *m_toolBarTafesir;
    QComboBox *m_comboTafasir;
    QToolBar *m_toolBarSearch;
    FilterLineEdit *m_searchEdit;
    QAction *m_searchPrevAction;
    QAction *m_searchNextAction;
};

#endif // BOOKSVIEWER_H
