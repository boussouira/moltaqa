#ifndef BOOKREFEREDIALOG_H
#define BOOKREFEREDIALOG_H

#include <QDialog>

namespace Ui {
class BookRefereDialog;
}

class QStandardItemModel;

class BookRefereDialog : public QDialog
{
    Q_OBJECT
    
public:
    BookRefereDialog(QWidget *parent = 0);
    ~BookRefereDialog();
    
protected:
    void loadModel();

private slots:
    void on_toolAdd_clicked();

    void on_toolEdit_clicked();

    void on_toolDelete_clicked();

private:
    QStandardItemModel *m_model;
    Ui::BookRefereDialog *ui;
};

#endif // BOOKREFEREDIALOG_H
