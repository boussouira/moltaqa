#ifndef BOOKWIDGETMANAGER_H
#define BOOKWIDGETMANAGER_H

#include <qwidget.h>
#include <qicon.h>

class BookWidget;
class LibraryManager;
class TabWidget;
class QSplitter;
class LibraryBook;
class RichBookReader;

class BookWidgetManager : public QWidget
{
    Q_OBJECT
public:
    BookWidgetManager(QWidget *parent = 0);
    ~BookWidgetManager();

    int addBook(BookWidget* book);
    BookWidget *bookWidget(int index);
    BookWidget *activeBookWidget();
    LibraryBook *activeBook();
    RichBookReader *activeBookReader();
    TabWidget *activeTab();
    TabWidget *unActiveTab();
    void setActiveTab(QObject *obj);

protected:
    void setupActions();
    void connectTab(TabWidget *tab);
    void reverseActiveTab();

public slots:
    void tabChanged(int newIndex);
    void tabCloseRequest(int tabIndex);
    void changeActiveTab();
    void moveToOtherTab(int index);
    void reverseSplitter();

    void nextAya();
    void previousAya();
    void nextPage();
    void previousPage();
    void firstPage();
    void lastPage();
    void goToPage();

protected slots:
    void changeTabIcon(QIcon tabIcon);

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
    bool m_showOtherTab;
};

#endif // BOOKWIDGETMANAGER_H
