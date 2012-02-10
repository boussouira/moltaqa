#ifndef SELECTCATDIALOG_H
#define SELECTCATDIALOG_H

#include <QDialog>

namespace Ui {
    class selectCatDialog;
}

class LibraryManager;
class QStandardItemModel;
class QStandardItem;
class SortFilterProxyModel;

class selectCatDialog : public QDialog
{
    Q_OBJECT

public:
    selectCatDialog(QWidget *parent = 0);
    ~selectCatDialog();

    QStandardItem *selectedNode();
    QString selectedCatName();
    int selectedCatID();

protected slots:
    void selectCat();
    void cancel();

protected:
    LibraryManager *m_libraryManager;
    QStandardItemModel *m_model;
    SortFilterProxyModel *m_filter;
    QStandardItem *m_selectedItem;

signals:
    void catSelected();

private:
    Ui::selectCatDialog *ui;
};

#endif // SELECTCATDIALOG_H
