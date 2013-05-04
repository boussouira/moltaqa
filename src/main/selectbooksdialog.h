#ifndef SELECTBOOKSDIALOG_H
#define SELECTBOOKSDIALOG_H

#include "librarybookmanager.h"

#include <qdialog.h>

namespace Ui {
class selectBooksDialog;
}

class ModelViewFilter;
class QModelIndex;

class selectBooksDialog : public QDialog
{
    Q_OBJECT
    
public:
    selectBooksDialog(QWidget *parent = 0);
    ~selectBooksDialog();
    
    QList<int> selectedBooks() { return m_selectedBooks; }

protected slots:
    void selectBooks();
    void cancel();

private:
    Ui::selectBooksDialog *ui;
    StandardItemModelPtr m_model;
    ModelViewFilter *m_filter;
    QList<int> m_selectedBooks;
};

#endif // SELECTBOOKSDIALOG_H
