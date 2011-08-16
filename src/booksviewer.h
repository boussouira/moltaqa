#ifndef BOOKSVIEWER_H
#define BOOKSVIEWER_H

#include <qwidget.h>

class BookWidget;
class IndexDB;
class TabWidget;
class QMainWindow;
class QToolBar;
class QMenu;
class QComboBox;

class BooksViewer : public QWidget
{
    Q_OBJECT
public:
    BooksViewer(IndexDB *indexDB, QMainWindow *parent);
    ~BooksViewer();
    void setIndexDB(IndexDB *indexDB) { m_indexDB = indexDB;}

public slots:
    void openBook(int pBookID, bool newTab = true);
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
    BookWidget *currentBookWidget();
    BookWidget *currentBookWidget(int index);
    void tabChanged(int newIndex);

protected:
    void createMenus(QMainWindow *parent);

signals:
    void lastTabClosed();

private:
    TabWidget *m_tab;
    IndexDB *m_indexDB;
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
    QAction *m_openSelectedTafsir;
    QToolBar *m_toolBarGeneral;
    QToolBar *m_toolBarNavigation;
    QToolBar *m_toolBarTafesir;
    QAction *m_navMenu;
    QComboBox *m_comboTafasir;
};

#endif // BOOKSVIEWER_H
