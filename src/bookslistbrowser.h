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
    BooksListBrowser(QWidget *parent = 0);
    ~BooksListBrowser();

private slots:
    void on_treeView_doubleClicked(QModelIndex index);
    void on_pushButton_clicked();
    void showBooksList();

signals:
    void bookSelected(int bookID);

private:
    BooksIndexDB *m_infoDB;
    Ui::BooksListBrowser *ui;
};

#endif // BOOKSLISTBROWSER_H
