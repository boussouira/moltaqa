#ifndef SELECTCATDIALOG_H
#define SELECTCATDIALOG_H

#include <QDialog>

namespace Ui {
    class selectCatDialog;
}

class IndexDB;
class BooksListModel;
class BooksListNode;
class SortFilterProxyModel;

class selectCatDialog : public QDialog
{
    Q_OBJECT

public:
    selectCatDialog(QWidget *parent = 0);
    ~selectCatDialog();

    BooksListNode *selectedNode();
    QString selectedCatName();
    int selectedCatID();

protected slots:
    void selectCat();
    void cancel();

protected:
    IndexDB *m_indexDB;
    BooksListModel *m_model;
    SortFilterProxyModel *m_filter;
    BooksListNode *m_selectedNode;

signals:
    void catSelected();

private:
    Ui::selectCatDialog *ui;
};

#endif // SELECTCATDIALOG_H
