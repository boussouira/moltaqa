#ifndef LIBRARYSEARCHWIDGET_H
#define LIBRARYSEARCHWIDGET_H

#include "searchwidget.h"
#include "resultwidget.h"

namespace Ui {
    class LibrarySearchWidget;
}

namespace lucene {
    namespace search {
        class Query;
    }
}

class SearchFilterManager;

class LibrarySearchWidget : public SearchWidget
{
    Q_OBJECT

public:
    LibrarySearchWidget(QWidget *parent = 0);
    ~LibrarySearchWidget();

    void setCurrentWidget(CurrentWidget index);
    void toggleWidget();

protected:
    void setupCleanMenu();
    lucene::search::Query *getSearchQuery();
    void initWebView();

protected slots:
    void clearSpecialChar();
    void clearLineText();
    void showFilterTools();
    void search();

private:
    Ui::LibrarySearchWidget *ui;
    SearchFilterManager *m_filterManager;
    ResultWidget *m_resultWidget;
    LibrarySearcher *m_searcher;
};

#endif // LIBRARYSEARCHWIDGET_H
