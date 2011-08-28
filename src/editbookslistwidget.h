#ifndef EDITBOOKSLISTWIDGET_H
#define EDITBOOKSLISTWIDGET_H

#include "abstracteditwidget.h"
#include <qhash.h>
#include <QModelIndex>

namespace Ui {
    class EditBooksListWidget;
}

class IndexDB;
class BookInfo;
class EditableBooksListModel;

class EditBooksListWidget : public AbstractEditWidget
{
    Q_OBJECT

public:
    EditBooksListWidget(QWidget *parent = 0);
    ~EditBooksListWidget();

    void enableEditWidgets(bool enable);

protected:
    void setupActions();
    void saveCurrentBookInfo();
    BookInfo *getBookInfo(int bookID);

public slots:
    void save();

private slots:
    void on_treeView_doubleClicked(const QModelIndex &index);
    void on_toolChangeAuthor_clicked();
    void editted();

protected:
    Ui::EditBooksListWidget *ui;
    IndexDB *m_indexDB;
    BookInfo *m_bookInfo;
    EditableBooksListModel *m_booksModel;
    QHash<int, BookInfo *> m_editedBookInfo;
};

#endif // EDITBOOKSLISTWIDGET_H
