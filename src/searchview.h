#ifndef SEARCHVIEW_H
#define SEARCHVIEW_H

#include "abstarctview.h"
#include "searchwidget.h"

class TabWidget;
class QVBoxLayout;

class SearchView : public AbstarctView
{
    Q_OBJECT

public:
    SearchView(QWidget *parent=0);

    QString title();
    void aboutToShow();

    bool canSearch(bool showMessage=true);

public slots:
    void newTab(SearchWidget::SearchType searchType, int bookID=0);
    void switchSearchWidget();

protected slots:
    void openNewTab();

signals:
    void lastTabClosed();

protected:
    TabWidget *m_tabWidget;
    QVBoxLayout *m_layout;
};

#endif // SEARCHVIEW_H
