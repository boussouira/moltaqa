#ifndef RESULTWIDGET_H
#define RESULTWIDGET_H

#include <QWidget>
#include "librarysearcher.h"

namespace Ui {
class ResultWidget;
}

class BooksViewer;
class WebView;

class ResultWidget : public QWidget
{
    Q_OBJECT

public:
    ResultWidget(QWidget *parent = 0);
    ~ResultWidget();

    void search(LibrarySearcher *searcher);

protected:
    void setupBookReaderView();
    void setupWebView();
    void showProgressBar(bool show);

public slots:
    void openResult(int resultID);
    void goToPage(int page);

private slots:
    void searchStarted();
    void searchFinnished();
    void fetechStarted();
    void fetechFinnished();
    void gotResult(SearchResult *result);
    void gotException(QString what, int id);
    void populateJavaScriptWindowObject();

    void ensureReaderVisible();
    void ensureReaderHidden(bool accessible=true);
    void maximizeBookReader();
    void minimizeBookReader();
    void lastTabClosed();

private:
    Ui::ResultWidget *ui;
    BooksViewer *m_readerview;
    QWidget *m_readerWidget;
    WebView *m_view;
    LibrarySearcher *m_searcher;
};

#endif // RESULTWIDGET_H
