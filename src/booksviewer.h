#ifndef BOOKSVIEWER_H
#define BOOKSVIEWER_H

#include <qwidget.h>

class BookWidget;
class IndexDB;
class TabWidget;
class QMainWindow;
class QToolBar;
class QComboBox;

class BooksViewer : public QWidget
{
    Q_OBJECT
public:
    BooksViewer(IndexDB *indexDB, QWidget *parent = 0);
    ~BooksViewer();
    void setIndexDB(IndexDB *indexDB) { m_indexDB = indexDB;}

public slots:
    void openBook(int pBookID, bool newTab = true);
    void nextUnit();
    void previousUnit();
    void nextPage();
    void previousPage();
    void updateActions();
    void showIndexWidget();
    void tabChangePosition(int fromIndex, int toIndex);
    void tabCloseRequest(int tabIndex);
    void createMenus(QMainWindow *parent);
    BookWidget *currentBookWidget();
    BookWidget *currentBookWidget(int index);
    void tabChanged(int newIndex);

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
    QAction *openSelectedTafsir;
    QToolBar *toolBarGeneral;
    QToolBar *toolBarNavigation;
    QToolBar *toolBarTafesir;
    QComboBox *comboTafasir;
};

#endif // BOOKSVIEWER_H
