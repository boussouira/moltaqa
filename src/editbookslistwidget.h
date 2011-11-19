#ifndef EDITBOOKSLISTWIDGET_H
#define EDITBOOKSLISTWIDGET_H

#include "abstracteditwidget.h"
#include <qhash.h>
#include <QModelIndex>

namespace Ui {
    class EditBooksListWidget;
}

class LibraryManager;
class LibraryBook;
class EditableBooksListModel;

class EditBooksListWidget : public AbstractEditWidget
{
    Q_OBJECT

public:
    EditBooksListWidget(QWidget *parent = 0);
    ~EditBooksListWidget();

    void enableEditWidgets(bool enable);

protected:
    void loadModel();
    void setupActions();
    void saveCurrentBookInfo();
    LibraryBook *getBookInfo(int bookID);

public slots:
    void save();

private slots:
    void on_treeView_doubleClicked(const QModelIndex &index);
    void on_toolChangeAuthor_clicked();
    void setupEdit(LibraryBook *info);
    void editted();

protected:
    Ui::EditBooksListWidget *ui;
    LibraryManager *m_libraryManager;
    LibraryBook *m_bookInfo;
    EditableBooksListModel *m_booksModel;
    QHash<int, LibraryBook *> m_editedBookInfo;
};

#endif // EDITBOOKSLISTWIDGET_H
