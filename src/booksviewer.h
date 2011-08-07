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
    QAction *actionNewTab;
    QAction *actionIndexDock;
    QAction *actionSearchDock;
    QAction *actionNextAYA;
    QAction *actionNextPage;
    QAction *actionPrevAYA;
    QAction *actionPrevPage;
    QAction *actionFirstPage;
    QAction *actionLastPage;
    QAction *actionGotToPage;
    QAction *openSelectedTafsir;
    QToolBar *toolBarGeneral;
    QToolBar *toolBarNavigation;
    QToolBar *toolBarTafesir;
    QAction *m_navMenu;
    QComboBox *comboTafasir;
};

#endif // BOOKSVIEWER_H
