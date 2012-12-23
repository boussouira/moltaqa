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

    inline void setLibraryBook(LibraryBook::Ptr book) { m_book = book; }
    void setup();

private slots:
    void on_treeView_doubleClicked(const QModelIndex &index);
    void deleteBookHistory();

signals:
    void openPage(int pageID);

private:
    Ui::BookHistoryDialog *ui;
    LibraryBook::Ptr m_book;
    StandardItemModelPtr m_model;
};

#endif // BOOKHISTORYDIALOG_H
