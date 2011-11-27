#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <qwidget.h>

namespace Ui {
    class SearchWidget;
}

namespace lucene {
    namespace search {
        class Query;
    }
}

class ResultWidget;
class SearchFilterManager;
class LibrarySearcher;
class SearchFilter;

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

    enum SearchType {
        LibrarySearch,
        BookSearch
    };

    virtual void init()=0;

    void setCurrentWidget(CurrentWidget index);
    void toggleWidget();

protected:
    virtual lucene::search::Query *getSearchQuery()=0;
    virtual SearchFilter *getSearchFilterQuery()=0;

public slots:
    void search();

    virtual void selectAll() {}
    virtual void unSelectAll() {}
    virtual void selectVisible() {}
    virtual void unSelectVisible() {}
    virtual void expandFilterView() {}
    virtual void collapseFilterView() {}

protected slots:
    void setupCleanMenu();
    void clearSpecialChar();
    void clearLineText();
    void showFilterTools();

protected:
    ResultWidget *m_resultWidget;
    LibrarySearcher *m_searcher;
    Ui::SearchWidget *ui;
};

#endif // SEARCHWIDGET_H
