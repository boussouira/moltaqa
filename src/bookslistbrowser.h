#ifndef BOOKSLISTBROWSER_H
#define BOOKSLISTBROWSER_H

#include <qdialog.h>
#include <qsqldatabase.h>
#include "qabstractitemmodel.h"

class BooksListModel;
class BooksListNode;

namespace Ui {
    class BooksListBrowser;
}

class BooksListBrowser : public QDialog {
    Q_OBJECT
public:
    BooksListBrowser(QWidget *parent = 0, bool showBooks = true);
    ~BooksListBrowser();
    void booksCat(BooksListNode *parentNode, int catID);
    void childCats(BooksListNode *parentNode, int pID);
    void setShowBooks(bool show) { m_showBooks = show; }
    void hideViewHeaders(bool hide);
    QString lastSelectedName() { return m_lastSelected; }
    int lastSelectedID() { return m_lastSelectedID; }

private slots:
    void on_treeView_doubleClicked(QModelIndex index);
    void on_pushButton_clicked();
    void showBooksList();

signals:
    void bookSelected(int bookID);

private:
    QSqlDatabase m_booksListDB;
    QString m_appDir;
    QString m_booksFolder;
    QString m_indexDBName;
    QString m_lastSelected;
    BooksListModel *m_listModel;
    bool m_showBooks;
    int m_lastSelectedID;
    Ui::BooksListBrowser *ui;
};

#endif // BOOKSLISTBROWSER_H
