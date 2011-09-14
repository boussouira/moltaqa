#ifndef BOOKSLISTBROWSER_H
#define BOOKSLISTBROWSER_H

#include <qdialog.h>
#include <qsqldatabase.h>
#include "bookslistmodel.h"

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
    void setIndexDB(IndexDB *indexDB) { m_indexDB = indexDB;}

protected:
    void closeEvent(QCloseEvent *event);
    void loadSettings();

public slots:
    void setModel(BooksListModel *model);

private slots:
    void on_treeView_doubleClicked(QModelIndex index);
    void on_pushButton_clicked();

signals:
    void bookSelected(int bookID);

private:
    IndexDB *m_indexDB;
    BooksListModel *m_model;
    SortFilterProxyModel *m_filterModel;
    Ui::BooksListBrowser *ui;
};

#endif // BOOKSLISTBROWSER_H
