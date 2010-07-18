#ifndef BOOKSVIEWER_H
#define BOOKSVIEWER_H

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QComboBox>
#include <QStackedWidget>

#include "ktab.h"
#include "indexwidget.h"
#include "bookslistbrowser.h"
#include "simpledbhandler.h"
#include "qurandbhandler.h"
#include "bookinfohandler.h"
#include "bookwidget.h"

class BooksViewer : public QWidget
{
    Q_OBJECT
public:
    BooksViewer(QWidget *parent = 0);
    ~BooksViewer();

public slots:
    void openBook(int pBookID, bool newTab = true);
    void nextUnit();
    void previousUnit();
    void nextPage();
    void previousPage();
    void showIndexWidget();
    void tabChangePosition(int fromIndex, int toIndex);
    void tabCloseRequest(int tabIndex);
    void createMenus(QMainWindow *parent);
    BookWidget *currentBookWidget();
    BookWidget *currentBookWidget(int index);
    void tabChanged(int newIndex);

protected:
    void updateActions();

private:
    KTab *m_tab;
    BookInfoHandler *m_infoDB;
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
