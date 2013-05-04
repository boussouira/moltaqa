#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <qtabwidget.h>

class BookViewBase;
class QTabBar;

class TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    TabWidget(QWidget *parent = 0);
    int addBookWidget(BookViewBase *book);
    void setCanMoveToOtherTabWidget(bool canMove);
    void setEnableTabBar(bool enable);
    void setAutoTabClose(bool autoClose);
    void setCloseLastTab(bool closeLast);

    void setTabBarActions(QList<QAction*> list);
    QList<QAction*> tabActions() { return m_tabBarActions; }

protected:
    bool eventFilter(QObject *obj, QEvent *event);

protected slots:
    void showTabBarMenu(QPoint point);
    void closeTab(int index);
    void setTabClosable();

signals:
    void tabMoved(int from, int to);
    void lastTabClosed();
    void gotFocus();
    void moveToOtherTab(int index);
    void reverseSplitter();

private:
    QTabBar *m_tabBar;
    QList<QAction*> m_tabBarActions;
    bool m_canMoveToOtherTabWidget;
    bool m_autoClose;
    bool m_closeLastTab;
};

#endif // TABWIDGET_H
