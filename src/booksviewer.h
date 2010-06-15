#ifndef BOOKSVIEWER_H
#define BOOKSVIEWER_H

#include <QMainWindow>
#include <QtWebKit>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QComboBox>
#include <QStackedWidget>

#include "ktab.h"
#include "indexwidget.h"
#include "qurantextmodel.h"
#include "tafessirtextbase.h"
#include "quransearch.h"
#include "bookslistbrowser.h"
#include "simpledbhandler.h"

class BooksViewer : public QMainWindow
{
    Q_OBJECT
public:
    BooksViewer(QMainWindow *parent = 0);
    ~BooksViewer();

public slots:
    void openBook(int pBookID);
    void openSora(int pSoraNumber, int pAyaNumber = 1);
    void displayPage(PageInfo *pPageInfo);
    void scrollToAya(int pSoraNumber, int pAyaNumber);
    void openSoraInNewTab(int pSoraNumber = 1);
    void ayaNumberChange(int pNewAyaNum);
    void updateSoraDetials();
    void nextAya();
    void previousAYA();
    void nextPage();
    void previousPage();
    void showIndexDock(bool pShowIndexDock);
    void showSearchDock(bool pShowSearchDock);
    void tabChangePosition(int fromIndex, int toIndex);
    void tabCloseRequest(int tabIndex);

protected:
    void updateNavigationButtons();
    void createMenus(QMainWindow *parent);
    IndexWidget *currentIndexWidget();
    QuranTextModel *databaseHandler();
    PageInfo *pageInfo();

private:
    KTab *m_tab;
    QuranSearch *m_quranSearch ;
    QStackedWidget *m_stackedWidget;
    QList<QuranTextModel *> m_databases;
    QDockWidget *m_quranSearchDock;
    QDockWidget *m_indexWidgetDock;
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
