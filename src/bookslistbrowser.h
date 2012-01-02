#ifndef BOOKSLISTBROWSER_H
#define BOOKSLISTBROWSER_H

#include <qdialog.h>
#include <qsqldatabase.h>
#include "bookslistmodel.h"

class LibraryManager;
class SortFilterProxyModel;

namespace Ui {
    class BooksListBrowser;
}

class BooksListBrowser : public QDialog {
    Q_OBJECT
public:
    BooksListBrowser(LibraryManager *libraryManager, QWidget *parent = 0);
    ~BooksListBrowser();
    void setLibraryManager(LibraryManager *libraryManager) { m_libraryManager = libraryManager;}

protected:
    void closeEvent(QCloseEvent *event);
    void loadSettings();

public slots:
    void setModel(BooksListModel *model);

private slots:
    void sortChanged(int logicalIndex, Qt::SortOrder);
    void on_treeView_doubleClicked(QModelIndex index);

signals:
    void bookSelected(int bookID);

private:
    LibraryManager *m_libraryManager;
    BooksListModel *m_model;
    SortFilterProxyModel *m_filterModel;
    Ui::BooksListBrowser *ui;
};

#endif // BOOKSLISTBROWSER_H
