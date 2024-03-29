#ifndef RESULTWIDGET_H
#define RESULTWIDGET_H

#include <qwidget.h>

namespace Ui {
class ResultWidget;
}

class BookReaderView;
class WebView;
class LibrarySearcher;
class SearchResult;

class ResultWidget : public QWidget
{
    Q_OBJECT

public:
    ResultWidget(QWidget *parent = 0);
    ~ResultWidget();

    WebView *resultWebView();

    void search(LibrarySearcher *searcher);

protected:
    void setupBookReaderView();
    void setupWebView();
    void showProgressBar(bool show);

public slots:
    void openResult(int resultID);
    void goToPage(int page);
    void showBookMenu(int bookID);

private slots:
    void searchStarted();
    void searchFinnished();
    void fetechStarted();
    void fetechFinnished();
    void gotResult(SearchResult *result);
    void gotException(QString what, int id);

    void ensureReaderVisible();
    void ensureReaderHidden(bool accessible=true);
    void maximizeBookReader();
    void minimizeBookReader();
    void lastTabClosed();
    void moveToReaderView();

private:
    Ui::ResultWidget *ui;
    BookReaderView *m_readerview;
    QWidget *m_readerWidget;
    WebView *m_view;
    LibrarySearcher *m_searcher;
    QAction *m_moveToReaderViewAct;
};

#endif // RESULTWIDGET_H
