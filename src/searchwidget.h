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
class QStandardItemModel;
class QCompleter;

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
        BookSearch,
        FavouritesSearch
    };

    virtual void init(int bookID=0)=0;

    void setCurrentWidget(CurrentWidget index);
    CurrentWidget currentWidget();
    void toggleWidget();

    void showSearchInfo();

    void setSearchText(QString mustQuery, QString shouldQuery="", QString mustNotQuery="");

protected:
    virtual lucene::search::Query *getSearchQuery(const wchar_t *searchField);
    virtual SearchFilter *getSearchFilterQuery()=0;
    virtual void loadDefaultSearchField();
    QString getSearchField();

    void loadSettings();
    void saveSettings();

    void saveSearchQuery();
    void loadSearchQuery();

public slots:
    void search();

protected slots:
    void setupCleanMenu();
    void clearSpecialChar();
    void showFilterTools();
    void showSearchFieldMenu();
    void doneSearching();

    void saveSelectedField();
    void searchfieldsDialog();
    void changeSearchfield();

protected:
    ResultWidget *m_resultWidget;
    LibrarySearcher *m_searcher;
    SearchFilterManager *m_filterManager;
    QStandardItemModel *m_completerModel;
    QCompleter *m_completer;
    Ui::SearchWidget *ui;
};

#endif // SEARCHWIDGET_H
