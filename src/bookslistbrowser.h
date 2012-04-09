#ifndef BOOKSLISTBROWSER_H
#define BOOKSLISTBROWSER_H

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qstandarditemmodel.h>

class BookListManager;
class LibraryManager;
class FavouritesManager;
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

public slots:
    void readBookListModel();
    void readFavouritesModel();

private slots:
    void itemClicked(QModelIndex index);

signals:
    void bookSelected(int bookID);

private:
    BookListManager *m_bookListManager;
    FavouritesManager *m_favouritesManager;
    QStandardItemModel *m_bookListModel;
    QStandardItemModel *m_favouritesModel;
    ModelViewFilter *m_bookListFilter;
    ModelViewFilter *m_favouritesListFilter;
    Ui::BooksListBrowser *ui;
};

#endif // BOOKSLISTBROWSER_H
