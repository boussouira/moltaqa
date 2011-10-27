#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <QWidget>

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

protected:
    void setupCleanMenu();
    lucene::search::Query *getSearchQuery();

public slots:
    void clearSpecialChar();
    void clearLineText();
    void testSearch();

private:
    Ui::SearchWidget *ui;
    SearchFilterManager *m_filterManager;
};

#endif // SEARCHWIDGET_H
