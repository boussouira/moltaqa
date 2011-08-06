#ifndef SHAMELAIMPORTDIALOG_H
#define SHAMELAIMPORTDIALOG_H

#include <QDialog>
#include "libraryinfo.h"

namespace Ui {
    class ShamelaImportDialog;
}

class ShamelaInfo;
class ShamelaManager;
class ShamelaImportThread;
class QStandardItemModel;

class ShamelaImportDialog : public QDialog
{
    Q_OBJECT

public:
    ShamelaImportDialog(QWidget *parent = 0);
    ~ShamelaImportDialog();

protected:
    QString getFolderPath(const QString &defaultPath);
    void openShamelaDB();
    /**
      Show page at the given index
      @param index Index of page, if it eqaul -1 then go to next page
      */
    void goPage(int index=-1);
    void showImportInfo();
    void startImporting();
    void showBooks();
    void createFilter();

public slots:
    void setStepTitle(const QString &title);
    void addDebugInfo(const QString &text);

private slots:
    void nextStep();
    void on_buttonSelectShamela_clicked();
    void doneImporting();
    void cancel();

signals:
    void setProgress(int value);

public:
    LibraryInfo *m_library;

private:
    ShamelaInfo *m_shamela;
    ShamelaManager *m_manager;
    ShamelaImportThread *m_importThread;
    QStandardItemModel *m_booksModel;
    Ui::ShamelaImportDialog *ui;
};

#endif // SHAMELAIMPORTDIALOG_H
