#ifndef SELECTAUTHORDIALOG_H
#define SELECTAUTHORDIALOG_H

#include <QDialog>

namespace Ui {
    class selectAuthorDialog;
}

class IndexDB;
class QStandardItemModel;
class SortFilterProxyModel;
class QModelIndex;

class selectAuthorDialog : public QDialog
{
    Q_OBJECT

public:
    selectAuthorDialog(QWidget *parent = 0);
    ~selectAuthorDialog();

    QString selectAuthorName();
    int selectAuthorID();

protected slots:
    void selectAuthor();
    void cancel();
    void on_treeView_doubleClicked(const QModelIndex &index);

protected:
    IndexDB *m_indexDB;
    QStandardItemModel *m_model;
    SortFilterProxyModel *m_filter;
    QString m_authorName;
    int m_authorID;

private:
    Ui::selectAuthorDialog *ui;
};

#endif // SELECTAUTHORDIALOG_H
