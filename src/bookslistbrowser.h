#ifndef BOOKSLISTBROWSER_H
#define BOOKSLISTBROWSER_H

#include <qdialog.h>
#include <qsqldatabase.h>
#include "qabstractitemmodel.h"

class IndexDB;
class SortFilterProxyModel;

namespace Ui {
    class BooksListBrowser;
}

class BooksListBrowser : public QDialog {
    Q_OBJECT
public:
    BooksListBrowser(IndexDB *indexDB, QWidget *parent = 0);
    ~BooksListBrowser();
    void setUpdateList(bool update) { m_updateList = update; }
    void setIndexDB(IndexDB *indexDB) { m_indexDB = indexDB;}

protected:
    void showEvent(QShowEvent* event);
    void closeEvent(QCloseEvent *event);
    void loadSettings();

public slots:
    void loadBooksList();

private slots:
    void on_treeView_doubleClicked(QModelIndex index);
    void on_pushButton_clicked();

signals:
    void bookSelected(int bookID);

private:
    bool m_updateList;
    IndexDB *m_indexDB;
    SortFilterProxyModel *m_filterModel;
    Ui::BooksListBrowser *ui;
};

#endif // BOOKSLISTBROWSER_H
