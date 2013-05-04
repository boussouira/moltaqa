#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <qfuturewatcher.h>
#include <qhash.h>
#include <qprogressdialog.h>
#include <qwizard.h>

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

class ImportDialog : public QWizard
{
    Q_OBJECT

public:
    ImportDialog(QWidget *parent = 0);
    ~ImportDialog();

    enum {
        Page_BookSelection,
        Page_ImportOption,
        Page_ImportedBooks
    };

    void addFile(const QString &path);
    void addDir(const QString &path);

    bool validateCurrentPage();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void closeEvent(QCloseEvent *event);

    bool fileExsistInList(const QString &path);
    bool checkNodes(QList<ImportModelNode *> nodesList);

    void convertBooks();
    void importBooks();

protected slots:
    void selectFiles();
    void deleteFiles();
    void nextPage();

    void doneConverting();

    void startImporting();
    void doneImporting();

    void bookConverted(QString bookName);
    void addBooksToProgress(int count);

signals:
    void openBook(int id);

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
    QProgressDialog m_progress;
};

#endif // IMPORTDIALOG_H
