#ifndef BOOKSLISTBROWSER_H
#define BOOKSLISTBROWSER_H

#include <qdialog.h>
#include <qsqldatabase.h>
#include "qabstractitemmodel.h"

class BooksIndexDB;

namespace Ui {
    class BooksListBrowser;
}

class BooksListBrowser : public QDialog {
    Q_OBJECT
public:
    BooksListBrowser(BooksIndexDB *indexDB, QWidget *parent = 0);
    ~BooksListBrowser();
    void setUpdateList(bool update) { m_updateList = update; }
    void setIndexDB(BooksIndexDB *indexDB) { m_indexDB = indexDB;}

protected:
    void showEvent(QShowEvent* event);

public slots:
    void showBooksList();

private slots:
    void on_treeView_doubleClicked(QModelIndex index);
    void on_pushButton_clicked();

signals:
    void bookSelected(int bookID);

private:
    BooksIndexDB *m_indexDB;
    Ui::BooksListBrowser *ui;
    bool m_updateList;
};

#endif // BOOKSLISTBROWSER_H
