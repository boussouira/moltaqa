#ifndef OPENPAGEDIALOG_H
#define OPENPAGEDIALOG_H

#include <QDialog>
#include "bookinfo.h"

namespace Ui {
    class OpenPageDialog;
}

class OpenPageDialog : public QDialog
{
    Q_OBJECT

public:
    OpenPageDialog(QWidget *parent = 0);
    ~OpenPageDialog();

    void setBookInfo(BookInfo *info);

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

private:
    int m_pageNum; ///< Selected page number
    int m_partNum; ///< Selected part number
    Ui::OpenPageDialog *ui;
};

#endif // OPENPAGEDIALOG_H
