#ifndef BOOKWIDGET_H
#define BOOKWIDGET_H

#include <qwidget.h>
#include <qfuture.h>
#include <qfuturewatcher.h>

class WebView;
class IndexWidget;
class RichBookReader;
class LibraryBookManager;
class BookReaderHelper;
class QSplitter;
class QVBoxLayout;
class QModelIndex;
class QStandardItemModel;

class BookWidget: public QWidget
{
    Q_OBJECT

public:
    BookWidget(RichBookReader *db, QWidget *parent=0);
    ~BookWidget();

    void displayInfo();
    RichBookReader *bookReader() { return m_db; }
    IndexWidget *indexWidget() { return m_indexWidget; }
    WebView *webView() { return m_view; }
    void hideIndexWidget();
    void saveSettings();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void focusInEvent(QFocusEvent *event);
    void loadSettings();

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
    RichBookReader *m_db;
    LibraryBookManager *m_bookManager;
    BookReaderHelper *m_bookHelper;
    QFuture<QStandardItemModel*> m_retModel;
    QFutureWatcher<QStandardItemModel*> m_watcher;
    bool m_viewInitialized;
    bool m_indexReading;
};

#endif // BOOKWIDGET_H
