#ifndef SEARCHQUERYWIDGET_H
#define SEARCHQUERYWIDGET_H

#include <qwidget.h>

namespace Ui {
class SearchQueryWidget;
}

namespace lucene {
namespace search {
class Query;
}
}

class QStringListModel;
class QCompleter;

class SearchQueryWidget : public QWidget
{
    Q_OBJECT
    
public:
    SearchQueryWidget(QWidget *parent = 0);
    ~SearchQueryWidget();

    lucene::search::Query *searchQuery(const wchar_t *searchField);

    void setSearchQuery(const QString &text);
    QString searchQueryStr(bool clean=false);

    void loadSettings();
    void saveSettings();

    void loadSearchQuery();
    void saveSearchQuery();

protected:
    void updateSearchWidget();
    lucene::search::Query *defaultQuery(const wchar_t *searchField);
    lucene::search::Query *advancedQuery(const wchar_t *searchField);

protected slots:
    void toggoleAdvancedSearchWidget();
    void setupCleanMenu();
    void clearSpecialChar();
    void matchSearch();
    void removeTashekil();

signals:
    void search();

private:
    Ui::SearchQueryWidget *ui;
    QStringListModel *m_completerModel;
    QCompleter *m_completer;
    QString m_searchQuery;
    bool m_advancedSearch;
};

#endif // SEARCHQUERYWIDGET_H
