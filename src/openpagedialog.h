#ifndef OPENPAGEDIALOG_H
#define OPENPAGEDIALOG_H

#include <qdialog.h>
#include "librarybook.h"

namespace Ui {
    class OpenPageDialog;
}

class OpenPageDialog : public QDialog
{
    Q_OBJECT

public:
    OpenPageDialog(QWidget *parent = 0);
    ~OpenPageDialog();

    void setInfo(LibraryBook::Ptr info, BookPage *page);

    int currentPage();

    int selectedPage();
    int selectedPart();

    int selectedSora();
    int selectedAya();

    int selectedHaddit();

protected:
    void loadSowarNames();

private slots:
    void on_pushCancel_clicked();
    void on_pushGo_clicked();
    void on_comboSora_currentIndexChanged(int index);
    void on_spinPart_editingFinished();

private:
    int m_pageNum; ///< Selected page number
    int m_partNum; ///< Selected part number
    LibraryBook::Ptr m_info;
    Ui::OpenPageDialog *ui;
};

#endif // OPENPAGEDIALOG_H
