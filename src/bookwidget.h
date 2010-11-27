#ifndef BOOKWIDGET_H
#define BOOKWIDGET_H

#include <qwidget.h>

class WebView;
class IndexWidget;
class AbstractDBHandler;
class QSplitter;
class QVBoxLayout;

class BookWidget: public QWidget
{
    Q_OBJECT

public:
    BookWidget(AbstractDBHandler *db, QWidget *parent=0);
    ~BookWidget();
    void setDBHandler(AbstractDBHandler *db);
    void displayInfo();
    AbstractDBHandler *dbHandler() { return m_db; }
    void firstPage();
    void nextPage();
    void prevPage();
    void nextUnit();
    void prevUnit();
    void hideIndexWidget();

public slots:
    void openID(int id);

protected:
    QSplitter *m_splitter;
    QVBoxLayout *m_layout;
    WebView *m_view;
    IndexWidget *m_indexWidget;
    AbstractDBHandler *m_db;
};

#endif // BOOKWIDGET_H
