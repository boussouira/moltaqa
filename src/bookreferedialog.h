#ifndef BOOKREFEREDIALOG_H
#define BOOKREFEREDIALOG_H

#include <qdialog.h>

namespace Ui {
class BookRefereDialog;
class AddBookReferDialog;
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
    void setupRefererEditor(Ui::AddBookReferDialog *editor);

private slots:
    void on_toolAdd_clicked();
    void on_toolEdit_clicked();
    void on_toolDelete_clicked();
    void addReferToEditor(QString href);

private:
    QStandardItemModel *m_model;
    Ui::AddBookReferDialog *m_referEditor;
    Ui::BookRefereDialog *ui;
};

#endif // BOOKREFEREDIALOG_H
