#ifndef BOOKWIDGET_H
#define BOOKWIDGET_H

#include <qwidget.h>
#include <qfuture.h>

class WebView;
class IndexWidget;
class RichBookReader;
class QSplitter;
class QVBoxLayout;
class BookIndexModel;
class QModelIndex;

class BookWidget: public QWidget
{
    Q_OBJECT

public:
    BookWidget(RichBookReader *db, QWidget *parent=0);
    ~BookWidget();

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
    bool eventFilter(QObject *obj, QEvent *event);
    void focusInEvent(QFocusEvent *event);
    void loadSettings();

public slots:
    void openPage(int id);
    void openPage(int pageNum, int partNum);
    void openSora(int sora, int aya);
    void openHaddit(int hadditNum);

    void readerTextChanged();

    void showIndex();
    void showIndex(int tid);

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
    bool m_viewInitialized;
};

#endif // BOOKWIDGET_H
