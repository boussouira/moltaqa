#ifndef BOOKWIDGETMANAGER_H
#define BOOKWIDGETMANAGER_H

#include <qwidget.h>
#include "librarybook.h"

class BookWidget;
class LibraryManager;
class TabWidget;
class QSplitter;
class AbstractBookReader;

class BookWidgetManager : public QWidget
{
    Q_OBJECT
public:
    BookWidgetManager(QWidget *parent = 0);
    ~BookWidgetManager();

    int addBook(BookWidget* book);
    BookWidget *bookWidget(int index);
    BookWidget *activeBookWidget();
    LibraryBook::Ptr activeBook();
    AbstractBookReader *activeBookReader();
    TabWidget *activeTab();
    TabWidget *unActiveTab();
    void setActiveTab(QObject *obj);

    void closeBook(int bookID);
    BookWidget *getBookWidget(int bookID);
    QList<BookWidget *> getBookWidgets();
    bool showBook(int bookID);

    void addTabActions(QList<QAction *> tabActions);

protected:
    void connectTab(TabWidget *tab);
    void reverseActiveTab();

public slots:
    void tabChanged(int newIndex);
    void tabCloseRequest(int tabIndex);
    void changeActiveTab();
    void moveToOtherTab();
    void reverseSplitter();
    void addToFavouite();
    void showBookInfo();
    void showBookHistory();

    void nextAya();
    void previousAya();
    void nextPage();
    void previousPage();
    void firstPage();
    void lastPage();
    void goToPage();

signals:
    void lastTabClosed();
    void currentTabChanged(int index);
    void pageChanged();

protected:
    TabWidget *m_topTab;
    TabWidget *m_bottomTab;
    TabWidget *m_activeTab;
    QSplitter *m_splitter;
    LibraryManager *m_libraryManager;
    QAction *m_moveAct;
    QAction *m_revAct;
    QAction *m_favouriteAct;
    QAction *m_bookInfoAct;
    bool m_showOtherTab;
};

#endif // BOOKWIDGETMANAGER_H
