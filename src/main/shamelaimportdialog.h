#ifndef SHAMELAIMPORTDIALOG_H
#define SHAMELAIMPORTDIALOG_H

#include <qdatetime.h>
#include <qicon.h>
#include <qwizard.h>

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
class LibraryInfo;

class ShamelaImportDialog : public QWizard
{
    Q_OBJECT

public:
    ShamelaImportDialog(QWidget *parent = 0);
    ~ShamelaImportDialog();

    enum {
        Page_ImportOption,
        Page_BookSelection,
        Page_CategoriesLink,
        Page_ImportProgress
    };

    static ShamelaImportDialog *instance();
    ShamelaManager *shamelaManager();
    ShamelaInfo *shamelaInfo();
    LibraryInfo *libraryInfo();

    void setLibraryInfo(LibraryInfo *info);

    bool validateCurrentPage();
    int nextId() const;

protected:
    void closeEvent(QCloseEvent *event);
    void initializePage(int id);

    void showBooks();
    bool createFilter();

    void setupCategories();
    bool categorieLinked();

    void setupImporting();
    void startImporting();

public slots:
    void addDebugInfo(const QString &text, QIcon icon=QIcon(":/images/about.png"));
    void bookImported(const QString &text);
    void BookImportError(const QString &text);

private slots:
    void doneImporting();
    void importShorooh();
    bool cancel();

    void itemChanged(QStandardItem *item);
    void selectAllBooks();
    void unSelectAllBooks();
    void selectNewBooks();
    int selectNewBook(QStandardItem *item, QProgressDialog *progress);

private:
    Ui::ShamelaImportDialog *ui;

    int m_importThreadCount;
    int m_importedBooksCount;
    QStringList m_importError; ///< Titles of non-imported books
    LibraryInfo *m_library;
    LibraryManager *m_libraryManager;
    BookListManager *m_bookListManager;
    LibraryBookManager *m_bookManager;
    ShamelaInfo *m_shamela;
    ShamelaManager *m_manager;
    QStandardItemModel *m_booksModel;
    ModelViewFilter *m_bookFilter;
    QList<ShamelaImportThread*> m_importThreads;
    QTime m_importTime;
    bool m_proccessItemChange;
};

#endif // SHAMELAIMPORTDIALOG_H
