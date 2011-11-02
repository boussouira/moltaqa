#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <QWidget>
#include "resultwidget.h"

namespace Ui {
    class SearchWidget;
}

namespace lucene {
    namespace search {
        class Query;
    }
}

class SearchFilterManager;

class SearchWidget : public QWidget
{
    Q_OBJECT

public:
    SearchWidget(QWidget *parent = 0);
    ~SearchWidget();

    enum CurrentWidget {
        Search,
        Result
    };

    void setCurrentWidget(CurrentWidget index);
    void toggleWidget();

protected:
    void setupCleanMenu();
    lucene::search::Query *getSearchQuery();
    void initWebView();

public slots:
    void clearSpecialChar();
    void clearLineText();
    void testSearch();
    void search();

private:
    Ui::SearchWidget *ui;
    SearchFilterManager *m_filterManager;
    ResultWidget *m_resultWidget;
    LibrarySearcher *m_searcher;
};

#endif // SEARCHWIDGET_H
