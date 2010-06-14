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

private slots:
    void loadBooks();
    BooksListNode *getNodeByDepth(BooksListNode *pNode, int pDepth);

private:
    Ui::BooksListBrowser *ui;
};

#endif // BOOKSLISTBROWSER_H
