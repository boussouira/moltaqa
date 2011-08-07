#ifndef OPENPAGEDIALOG_H
#define OPENPAGEDIALOG_H

#include <QDialog>

namespace Ui {
    class OpenPageDialog;
}

class OpenPageDialog : public QDialog
{
    Q_OBJECT

public:
    OpenPageDialog(QWidget *parent = 0);
    ~OpenPageDialog();

    void setPage(int page);
    void setPart(int part);
    int selectedPage();
    int selectedPart();

private slots:
    void on_pushCancel_clicked();
    void on_pushGo_clicked();

private:
    int m_pageNum; ///< Selected page number
    int m_partNum; ///< Selected part number
    Ui::OpenPageDialog *ui;
};

#endif // OPENPAGEDIALOG_H
