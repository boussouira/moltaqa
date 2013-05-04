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
    WebViewSearcher *searcher();

    bool canSearch(bool showMessage=true);

    SearchWidget *currentSearchWidget();

public slots:
    SearchWidget *newTab(SearchWidget::SearchType searchType, int bookID=0);
    void switchSearchWidget();

protected slots:
    void openNewTab();
    void searchAgain();
    void searchInfo();

signals:
    void lastTabClosed();

protected:
    TabWidget *m_tabWidget;
    QVBoxLayout *m_layout;
};

#endif // SEARCHVIEW_H
