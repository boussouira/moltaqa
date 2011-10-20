#ifndef BOOKWIDGETMANAGER_H
#define BOOKWIDGETMANAGER_H

#include <QWidget>

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

    void addBook(BookWidget* book);
    BookWidget *bookWidget(int index);
    BookWidget *activeBookWidget();
    LibraryBook *activeBook();
    RichBookReader *activeBookReader();
    TabWidget *activeTab();
    void setActiveTab(QObject *obj);

protected:
    void connectTab(TabWidget *tab);

public slots:
    void tabChanged(int newIndex);
    void tabCloseRequest(int tabIndex);
    void changeActiveTab();
    void moveToOtherTab(int index);
    void reverseSplitter();

signals:
    void lastTabClosed();
    void currentTabChanged(int index);

protected:
    TabWidget *m_topTab;
    TabWidget *m_bottomTab;
    TabWidget *m_activeTab;
    QSplitter *m_splitter;
    LibraryManager *m_libraryManager;
    bool m_showOtherTab;
};

#endif // BOOKWIDGETMANAGER_H
