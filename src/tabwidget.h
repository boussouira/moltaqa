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
    /**
      @brief Capture the signale when the tab is moved from an index position to an other one.

      When the tab has moved from an index postion to an other one, we need to do the same thing
      with the PageInfo contiand in m_sowarInfo.
      @param from   The previous position index.
      @param to     The new position index.
      */
    void tabIsMoved(int from, int to);
    void updateTabBar();

signals:
    /// this signal does the same thing as QTabBar::tabMoved().
    void tabMoved(int from, int to);

private:
    QTabBar *m_tab;

};

#endif // TABWIDGET_H
