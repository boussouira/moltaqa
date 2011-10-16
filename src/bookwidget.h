#ifndef BOOKWIDGET_H
#define BOOKWIDGET_H

#include <qwidget.h>
#include <qfuture.h>

class WebView;
class IndexWidget;
class RichBookReader;
class QSplitter;
class QVBoxLayout;
class QAbstractItemModel;

class BookWidget: public QWidget
{
    Q_OBJECT

public:
    BookWidget(RichBookReader *db, QWidget *parent=0);
    ~BookWidget();
    void setBookReader(RichBookReader *db);
    void displayInfo();
    RichBookReader *bookReader() { return m_db; }
    IndexWidget *indexWidget() { return m_indexWidget; }
    void firstPage();
    void lastPage();
    void nextPage();
    void prevPage();
    void scrollDown();
    void scrollUp();
    void hideIndexWidget();
    void saveSettings();

protected:
    void loadSettings();

public slots:
    void openPage(int id);
    void openPage(int pageNum, int partNum);
    void openSora(int sora, int aya);
    void openHaddit(int hadditNum);

    void indexModelReady();
    void readerTextChanged();

protected:
    QSplitter *m_splitter;
    QVBoxLayout *m_layout;
    WebView *m_view;
    IndexWidget *m_indexWidget;
    RichBookReader *m_db;
    QFuture<QAbstractItemModel*> m_retModel;
    QFutureWatcher<QAbstractItemModel*> *m_watcher;
};

#endif // BOOKWIDGET_H
