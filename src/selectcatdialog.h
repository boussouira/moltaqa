#ifndef SELECTCATDIALOG_H
#define SELECTCATDIALOG_H

#include <QDialog>

namespace Ui {
    class selectCatDialog;
}

class LibraryManager;
class QStandardItemModel;
class QStandardItem;
class ModelViewFilter;

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

signals:
    void catSelected();

private:
    LibraryManager *m_libraryManager;
    QStandardItemModel *m_model;
    ModelViewFilter *m_filter;
    QStandardItem *m_selectedItem;
    Ui::selectCatDialog *ui;
};

#endif // SELECTCATDIALOG_H
