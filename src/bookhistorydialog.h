#ifndef BOOKHISTORYDIALOG_H
#define BOOKHISTORYDIALOG_H

#include <qdialog.h>
#include "librarybook.h"
#include "librarybookmanager.h"

namespace Ui {
class BookHistoryDialog;
}

class QModelIndex;

class BookHistoryDialog : public QDialog
{
    Q_OBJECT
    
public:
    BookHistoryDialog(QWidget *parent = 0);
    ~BookHistoryDialog();

    inline void setLibraryBook(LibraryBookPtr book) { m_book = book; }
    void setup();

private slots:
    void on_treeView_doubleClicked(const QModelIndex &index);
    void deleteBookHistory();

signals:
    void openPage(int pageID);

private:
    Ui::BookHistoryDialog *ui;
    LibraryBookPtr m_book;
    StandardItemModelPtr m_model;
};

#endif // BOOKHISTORYDIALOG_H
