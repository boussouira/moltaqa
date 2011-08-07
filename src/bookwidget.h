#ifndef BOOKWIDGET_H
#define BOOKWIDGET_H

#include <qwidget.h>
#include <qfuture.h>

class WebView;
class IndexWidget;
class AbstractDBHandler;
class QSplitter;
class QVBoxLayout;
class QAbstractItemModel;

class BookWidget: public QWidget
{
    Q_OBJECT

public:
    BookWidget(AbstractDBHandler *db, QWidget *parent=0);
    ~BookWidget();
    void setDBHandler(AbstractDBHandler *db);
    void displayInfo();
    AbstractDBHandler *dbHandler() { return m_db; }
    IndexWidget *indexWidget() { return m_indexWidget; }
    void firstPage();
    void lastPage();
    void nextPage();
    void prevPage();
    void nextUnit();
    void prevUnit();
    void hideIndexWidget();

public slots:
    void openID(int id);
    void indexModelReady();

protected:
    QSplitter *m_splitter;
    QVBoxLayout *m_layout;
    WebView *m_view;
    IndexWidget *m_indexWidget;
    AbstractDBHandler *m_db;
    QList<int> m_splitterSizes;
    QFuture<QAbstractItemModel*> m_retModel;
    QFutureWatcher<QAbstractItemModel*> *m_watcher;
};

#endif // BOOKWIDGET_H
