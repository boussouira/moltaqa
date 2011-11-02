#ifndef SEARCHVIEW_H
#define SEARCHVIEW_H

#include "abstarctview.h"

class TabWidget;
class SearchWidget;
class QVBoxLayout;

class SearchView : public AbstarctView
{
    Q_OBJECT

public:
    SearchView(QWidget *parent=0);

    QString title();
    void ensureTabIsOpen();

public slots:
    void newTab();
    void closeTab(int index);
    void switchSearchWidget();

signals:
    void lastTabClosed();

protected:
    TabWidget *m_tabWidget;
    SearchWidget *m_searchWidget;
    QVBoxLayout *m_layout;
};

#endif // SEARCHVIEW_H
