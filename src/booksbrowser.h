#ifndef BOOKSBROWSER_H
#define BOOKSBROWSER_H

#include <QDialog>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "bookslistmodel.h"
#include "bookslistnode.h"

namespace Ui {
    class BooksBrowser;
}

class BooksBrowser : public QDialog {
    Q_OBJECT
public:
    BooksBrowser(QWidget *parent = 0);
    ~BooksBrowser();

private slots:
    void loadBooks();
    BooksListNode *getNodeByDepth(BooksListNode *pNode, int pDepth);

private:
    Ui::BooksBrowser *ui;
};

#endif // BOOKSBROWSER_H
