#ifndef SEARCHVIEW_H
#define SEARCHVIEW_H

#include "abstarctview.h"

class TabWidget;
class SearchWidget;
class QVBoxLayout;

class SearchView : public AbstarctView
{
public:
    SearchView(QWidget *parent=0);

    QString title();

protected:
    TabWidget *m_tabWidget;
    SearchWidget *m_searchWidget;
    QVBoxLayout *m_layout;
};

#endif // SEARCHVIEW_H
