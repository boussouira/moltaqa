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
    QString selectShamelBook();
    void getBookInfo(const QString &path, QList<ImportModelNode*> &nodes);
    QString getBookType(const QSqlDatabase &bookDB);

private:
    Ui::ImportDialog *ui;
    ImportModel *m_model;
    BooksIndexDB *m_indexDB;

private slots:
    void on_pushImport_clicked();
    void on_pushDelete_clicked();
    void on_pushAdd_clicked();
    void on_pushCancel_clicked();
};

#endif // IMPORTDIALOG_H
