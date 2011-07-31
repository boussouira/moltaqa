#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <qtabwidget.h>
#include <qtabbar.h>

class TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    TabWidget(QWidget *parent = 0);

public slots:
    void updateTabBar();

signals:
    void tabMoved(int from, int to);
    void lastTabClosed();

private:
    QTabBar *m_tabBar;

};

#endif // TABWIDGET_H
