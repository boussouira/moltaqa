#ifndef SEARCHFIELDSDIALOG_H
#define SEARCHFIELDSDIALOG_H

#include <qdialog.h>

namespace Ui {
    class SearchFieldsDialog;
}

class SearchManager;

class SearchFieldsDialog : public QDialog
{
    Q_OBJECT

public:
    SearchFieldsDialog(QWidget *parent = 0);
    ~SearchFieldsDialog();
    void loadSearchfields();

private slots:
    void on_toolEdit_clicked();
    void on_toolDelete_clicked();

protected:
    Ui::SearchFieldsDialog *ui;
    SearchManager *m_searchManager;
};

#endif // SEARCHFIELDSDIALOG_H
