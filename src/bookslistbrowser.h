#ifndef BOOKSLISTBROWSER_H
#define BOOKSLISTBROWSER_H

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qstandarditemmodel.h>

class BookListManager;
class LibraryManager;
class SortFilterProxyModel;

namespace Ui {
    class BooksListBrowser;
}

class BooksListBrowser : public QDialog {
    Q_OBJECT

public:
    BooksListBrowser(QWidget *parent = 0);
    ~BooksListBrowser();

protected:
    void closeEvent(QCloseEvent *event);
    void loadSettings();
    void saveSettings();

public slots:
    void readBookListModel();
    void setModel(QStandardItemModel *model);

private slots:
    void setFilterText(QString text);
    void sortChanged(int logicalIndex, Qt::SortOrder);
    void on_treeView_doubleClicked(QModelIndex index);

signals:
    void bookSelected(int bookID);

private:
    BookListManager *m_bookListManager;
    QStandardItemModel *m_model;
    SortFilterProxyModel *m_filterModel;
    Ui::BooksListBrowser *ui;
};

#endif // BOOKSLISTBROWSER_H
