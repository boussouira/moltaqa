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

#ifdef USE_MDBTOOLS
class MdbConverterManager;
#endif

class ShamelaImportDialog : public QDialog
{
    Q_OBJECT

public:
    ShamelaImportDialog(QWidget *parent = 0);
    ~ShamelaImportDialog();
    static ShamelaImportDialog *importDialog();
    ShamelaManager *shamelaManager();
    ShamelaInfo *shamelaInfo();
    LibraryInfo *libraryInfo();

#ifdef USE_MDBTOOLS
    MdbConverterManager *mdbManager();
#endif

    bool addAuthorsForEachBook();
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
    void showImportInfo();
    void setupImporting();
    void startImporting();
    void showBooks();
    void createFilter();

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

signals:
    void setProgress(int value);

private:
    int m_importThreadCount;
    int m_importedBooksCount;
    LibraryInfo *m_library;
    ShamelaInfo *m_shamela;
    ShamelaManager *m_manager;
    ShamelaImportThread *m_importThread;
    QStandardItemModel *m_booksModel;
    QList<ShamelaImportThread*> m_importThreads;
    Ui::ShamelaImportDialog *ui;
    QTime m_importTime;

#ifdef USE_MDBTOOLS
    MdbConverterManager *m_mdbManager;
#endif
};

#endif // SHAMELAIMPORTDIALOG_H
