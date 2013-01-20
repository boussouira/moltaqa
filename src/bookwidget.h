#ifndef BOOKWIDGET_H
#define BOOKWIDGET_H

#include <qwidget.h>
#include <qfuture.h>
#include <qfuturewatcher.h>
#include "librarybook.h"

class WebView;
class IndexWidget;
class RichBookReader;
class AbstractBookReader;
class LibraryBookManager;
class BookReaderHelper;
class QSplitter;
class QVBoxLayout;
class QModelIndex;
class QStandardItemModel;
class WebViewSearcher;

class BookWidget: public QWidget
{
    Q_OBJECT

public:
    BookWidget(LibraryBook::Ptr book, QWidget *parent=0);
    ~BookWidget();

    LibraryBook::Ptr book();
    AbstractBookReader *bookReader();
    WebViewSearcher *viewSearcher();

    void hideIndexWidget();

    void saveSettings();
    void loadSettings();

protected:
    void openReader();
    void loadIndexModel();

    bool eventFilter(QObject *obj, QEvent *event);
    void focusInEvent(QFocusEvent *event);

public slots:
    void firstPage();
    void lastPage();
    void nextPage();
    void prevPage();

    void wheelNextPage();
    void wheelPrevPage();

    void scrollDown();
    void scrollUp();

    void openPage(int id);
    void openPage(int pageNum, int partNum);
    void openSora(int sora, int aya);
    void openHaddit(int hadditNum);

    void scrollToCurrentAya(bool timer=false);

    void indexModelReady();
    void readerTextChanged();
    void reloadCurrentPage();

    void showIndex();
    void showIndex(int tid);
    QString getBreadcrumbs();

    bool search(const QString &text);
    void searchNext();
    void searchPrevious();

protected slots:
    void viewObjectCleared();

signals:
    void gotFocus();
    void textChanged();

protected:
    QSplitter *m_splitter;
    QByteArray m_splitterState;
    QVBoxLayout *m_layout;
    WebView *m_view;
    IndexWidget *m_indexWidget;
    LibraryBook::Ptr m_book;
    RichBookReader *m_reader;
    LibraryBookManager *m_bookManager;
    BookReaderHelper *m_bookHelper;
    QFuture<QStandardItemModel*> m_retModel;
    QFutureWatcher<QStandardItemModel*> m_watcher;
    bool m_viewInitialized;
    bool m_indexReading;
};

#endif // BOOKWIDGET_H
