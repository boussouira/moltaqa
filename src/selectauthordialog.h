#ifndef SELECTAUTHORDIALOG_H
#define SELECTAUTHORDIALOG_H

#include <QDialog>

namespace Ui {
    class selectAuthorDialog;
}

class QStandardItemModel;
class ModelViewFilter;
class QModelIndex;

class selectAuthorDialog : public QDialog
{
    Q_OBJECT

public:
    selectAuthorDialog(QWidget *parent = 0);
    ~selectAuthorDialog();

    QString selectedAuthorName();
    int selectedAuthorID();

protected slots:
    void selectAuthor();
    void cancel();
    void on_treeView_doubleClicked(const QModelIndex &index);

signals:
    void authorSelected();

protected:
    Ui::selectAuthorDialog *ui;
    QStandardItemModel *m_model;
    ModelViewFilter *m_filter;
    QString m_authorName;
    int m_authorID;
};

#endif // SELECTAUTHORDIALOG_H
