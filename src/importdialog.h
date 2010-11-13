#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <qdialog.h>

namespace Ui {
    class ImportDialog;
}
class ImportModel;
class ImportModelNode;
class BooksIndexDB;
class QSqlDatabase;

class ImportDialog : public QDialog {
    Q_OBJECT
public:
    ImportDialog(QWidget *parent = 0);
    ~ImportDialog();

protected:
    void getBookInfo(const QString &path, QList<ImportModelNode*> &nodes);
    QString getBookType(const QSqlDatabase &bookDB);
    bool checkNodes(QList<ImportModelNode *> nodesList);
    void convertBooks();
    void importBooks();

private:
    Ui::ImportDialog *ui;
    ImportModel *m_model;
    BooksIndexDB *m_indexDB;

private slots:
    void on_pushNext_clicked();
    void on_pushDeleteFile_clicked();
    void on_pushAddFile_clicked();
    void on_pushCancel_clicked();
};

#endif // IMPORTDIALOG_H
