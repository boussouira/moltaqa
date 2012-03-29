#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <qtabwidget.h>
#include <qtabbar.h>
#include "bookwidget.h"

class TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    TabWidget(QWidget *parent = 0);
    int addBookWidget(BookWidget *book);
    void setCanMoveToOtherTabWidget(bool canMove);
    void setEnableTabBar(bool enable);
    void setAutoTabClose(bool autoClose);
    void setCloseLastTab(bool closeLast);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

protected slots:
    void showTabBarMenu(QPoint point);
    void closeTab(int index);
    void setTabClosable();

signals:
    void tabMoved(int from, int to);
    void gotFocus();
    void moveToOtherTab(int index);
    void reverseSplitter();

private:
    QTabBar *m_tabBar;
    bool m_canMoveToOtherTabWidget;
    bool m_autoClose;
    bool m_closeLastTab;
};

#endif // TABWIDGET_H
