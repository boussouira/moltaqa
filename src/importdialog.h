#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <qdialog.h>

namespace Ui {
    class ImportDialog;
}
class ImportModel;
class ImportModelNode;
class IndexDB;
class QSqlDatabase;
class QSignalMapper;

class ImportDialog : public QDialog {
    Q_OBJECT
public:
    ImportDialog(IndexDB *indexDB, QWidget *parent = 0);
    ~ImportDialog();
    void setIndexDB(IndexDB *indexDB) { m_indexDB = indexDB;}
    static QString arPlural(int count, int word);
    void addFile(const QString &path);
    void addDir(const QString &path);

protected:
    bool checkNodes(QList<ImportModelNode *> nodesList);
    void convertBooks();
    void importBooks();
    bool fileExsistInList(const QString &);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

signals:
    void openBook(int id);
    void bookAdded();

protected slots:
    void doneConverting();
    void doneImporting();

private slots:
    void on_pushNext_clicked();
    void on_pushDeleteFile_clicked();
    void on_pushAddFile_clicked();
    void on_pushCancel_clicked();

private:
    Ui::ImportDialog *ui;
    ImportModel *m_model;
    IndexDB *m_indexDB;
    QSignalMapper *m_signalMapper;
};

#endif // IMPORTDIALOG_H
