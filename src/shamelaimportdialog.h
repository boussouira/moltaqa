#ifndef SHAMELAIMPORTDIALOG_H
#define SHAMELAIMPORTDIALOG_H

#include <QDialog>
#include <qdatetime.h>
#include "libraryinfo.h"

namespace Ui {
    class ShamelaImportDialog;
}

class ShamelaInfo;
class ShamelaManager;
class ShamelaImportThread;
class QStandardItemModel;
class QStandardItem;
class LibraryManager;
class LibraryBookManager;
class BookListManager;
class ModelViewFilter;
class QProgressDialog;

class ShamelaImportDialog : public QDialog
{
    Q_OBJECT

public:
    ShamelaImportDialog(QWidget *parent = 0);
    ~ShamelaImportDialog();
    static ShamelaImportDialog *instance();
    ShamelaManager *shamelaManager();
    ShamelaInfo *shamelaInfo();
    LibraryInfo *libraryInfo();

    void setLibraryInfo(LibraryInfo *info);

protected:
    void closeEvent(QCloseEvent *event);
    QString getFolderPath(const QString &defaultPath);
    void openShamelaDB();
    /**
      Show page at the given index
      @param index Index of page, if it eqaul -1 then go to next page
      */
    void goPage(int index=-1);
    void setupCategories();
    void setupImporting();
    void startImporting();
    void showBooks();
    bool createFilter();
    bool categorieLinked();

public slots:
    void setStepTitle(const QString &title);
    void addDebugInfo(const QString &text);
    void bookImported(const QString &text);

private slots:
    void nextStep();
    void selectShamela();
    void doneImporting();
    bool cancel();
    void importShorooh();

    void itemChanged(QStandardItem *item);
    void selectAllBooks();
    void unSelectAllBooks();
    void selectNewBooks();
    int selectNewBook(QStandardItem *item, QProgressDialog *progress);

private:
    int m_importThreadCount;
    int m_importedBooksCount;
    LibraryInfo *m_library;
    LibraryManager *m_libraryManager;
    BookListManager *m_bookListManager;
    LibraryBookManager *m_bookManager;
    ShamelaInfo *m_shamela;
    ShamelaManager *m_manager;
    QStandardItemModel *m_booksModel;
    ModelViewFilter *m_bookFilter;
    QList<ShamelaImportThread*> m_importThreads;
    Ui::ShamelaImportDialog *ui;
    QTime m_importTime;
    bool m_proccessItemChange;
};

#endif // SHAMELAIMPORTDIALOG_H
