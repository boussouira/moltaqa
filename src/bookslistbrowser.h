#ifndef BOOKSLISTBROWSER_H
#define BOOKSLISTBROWSER_H

#include <QDialog>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "bookslistmodel.h"
#include "bookslistnode.h"

namespace Ui {
    class BooksListBrowser;
}

class BooksListBrowser : public QDialog {
    Q_OBJECT
public:
    BooksListBrowser(QWidget *parent = 0);
    ~BooksListBrowser();
    void booksCat(BooksListNode *parentNode, int catID);
    void childCats(BooksListNode *parentNode, int pID);

private slots:
    void on_pushButton_clicked();
    void showBooksList();

private:
    QSqlDatabase m_booksListDB;
    Ui::BooksListBrowser *ui;
};

#endif // BOOKSLISTBROWSER_H
