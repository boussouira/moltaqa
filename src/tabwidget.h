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

protected:
    bool eventFilter(QObject *obj, QEvent *event);

protected slots:
    void showTabBarMenu(QPoint point);

signals:
    void tabMoved(int from, int to);
    void gotFocus();
    void moveToOtherTab(int index);
    void reverseSplitter();

private:
    QTabBar *m_tabBar;
};

#endif // TABWIDGET_H
