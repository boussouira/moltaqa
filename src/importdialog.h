#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <qdialog.h>
#include <qhash.h>
#include <qfuturewatcher.h>

namespace Ui {
    class ImportDialog;
}
class ImportModel;
class ImportModelNode;
class LibraryManager;
class AuthorDelegate;
class BookTypeDelegate;
class CategorieDelegate;
class QSignalMapper;
class QSqlDatabase;

class ImportDialog : public QDialog {
    Q_OBJECT
public:
    ImportDialog(QWidget *parent = 0);
    ~ImportDialog();

    void addFile(const QString &path);
    void addDir(const QString &path);

protected:
    bool checkNodes(QList<ImportModelNode *> nodesList);
    void convertBooks();
    void importBooks();
    bool fileExsistInList(const QString &);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void closeEvent(QCloseEvent *event);

signals:
    void openBook(int id);

protected slots:
    void doneConverting();
    void startImporting();
    void doneImporting();

private slots:
    void on_pushNext_clicked();
    void on_pushDeleteFile_clicked();
    void on_pushAddFile_clicked();
    void on_pushCancel_clicked();

private:
    Ui::ImportDialog *ui;
    QStringList m_acceptedTypes;
    ImportModel *m_model;
    LibraryManager *m_libraryManager;
    AuthorDelegate *m_authorDelegate;
    BookTypeDelegate *m_bookTypeDelegate;
    CategorieDelegate *m_categorieDelegate;
    QHash<int, QString> m_booksList;
    QFutureWatcher<void> m_importWatcher;
    QSignalMapper *m_signalMapper;
};

#endif // IMPORTDIALOG_H
