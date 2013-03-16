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

    enum TreeViewModel {
        CategoriesModel,
        FavoritesModel,
        LastOpenModel,
        AllBooksModel
    };

    void setCurrentModel(TreeViewModel model=CategoriesModel);

protected:
    void closeEvent(QCloseEvent *event);
    void loadSettings();
    void saveSettings();
    void setupListFilter(QStandardItemModel *sourceModel);

protected slots:
    void currentListChanged(int index);
    void listSortingChanged(int index);
    void sortOrderChanged(bool checked);

    void getCategoriesModel();
    void getFavouritesModel();
    void getRecentOpenModel(bool setupFilter);
    void getAllBooksModel(bool setupFilter);

    void itemClicked(QModelIndex index);
    void bookListMenu(QPoint);

signals:
    void bookSelected(int bookID, int pageID=-1);

private:
    BookListManager *m_bookListManager;
    FavouritesManager *m_favouritesManager;
    LibraryBookManager *m_bookManager;
    QStandardItemModel *m_currentModel;
    QStandardItemModel *m_categoriesModel;
    QStandardItemModel *m_favouritesModel;
    QStandardItemModel *m_recentOpenModel;
    QStandardItemModel *m_allBooksModel;
    ModelViewFilter *m_bookListFilter;
    Ui::BooksListBrowser *ui;
};

#endif // BOOKSLISTBROWSER_H
