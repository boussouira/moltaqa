#ifndef BOOKSLISTBROWSER_H
#define BOOKSLISTBROWSER_H

#include <qdialog.h>
#include <qstandarditemmodel.h>

class BookListManager;
class LibraryManager;
class FavouritesManager;
class LibraryBookManager;
class ModelViewFilter;
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

protected slots:
    void readBookListModel();
    void readFavouritesModel();
    void lastReadBooksModel();
    void itemClicked(QModelIndex index);
    void lastOpenedItemClicked(QModelIndex index);
    void bookListMenu(QPoint);

signals:
    void bookSelected(int bookID);

private:
    BookListManager *m_bookListManager;
    FavouritesManager *m_favouritesManager;
    LibraryBookManager *m_bookManager;
    QStandardItemModel *m_bookListModel;
    QStandardItemModel *m_favouritesModel;
    QStandardItemModel *m_lastReadedModel;
    ModelViewFilter *m_bookListFilter;
    ModelViewFilter *m_favouritesListFilter;
    Ui::BooksListBrowser *ui;
};

#endif // BOOKSLISTBROWSER_H
