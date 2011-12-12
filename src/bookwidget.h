#ifndef BOOKWIDGET_H
#define BOOKWIDGET_H

#include <qwidget.h>
#include <qfuture.h>
#include <qicon.h>

class WebView;
class IndexWidget;
class RichBookReader;
class QSplitter;
class QVBoxLayout;
class BookIndexModel;

class BookWidget: public QWidget
{
    Q_OBJECT

public:
    BookWidget(RichBookReader *db, QWidget *parent=0);
    ~BookWidget();

    enum TabIcon{
        Loading,
        Ready
    };

    void displayInfo();
    RichBookReader *bookReader() { return m_db; }
    IndexWidget *indexWidget() { return m_indexWidget; }
    QIcon icon() { return m_icon; }
    void firstPage();
    void lastPage();
    void nextPage();
    void prevPage();
    void scrollDown();
    void scrollUp();
    void hideIndexWidget();
    void saveSettings();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void focusInEvent(QFocusEvent *event);
    void loadSettings();
    void changeIcon(TabIcon iconType);

public slots:
    void openPage(int id);
    void openPage(int pageNum, int partNum);
    void openSora(int sora, int aya);
    void openHaddit(int hadditNum);

    void indexModelReady();
    void readerTextChanged();

signals:
    void setIcon(QIcon icon);
    void gotFocus();
    void textChanged();

protected:
    QSplitter *m_splitter;
    QVBoxLayout *m_layout;
    WebView *m_view;
    IndexWidget *m_indexWidget;
    RichBookReader *m_db;
    QFuture<BookIndexModel*> m_retModel;
    QFutureWatcher<BookIndexModel*> *m_watcher;
    QIcon m_icon;
    bool m_viewInitialized;
};

#endif // BOOKWIDGET_H
