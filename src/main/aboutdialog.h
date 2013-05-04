#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <qdialog.h>

namespace Ui {
    class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

    static void printVersion();

private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
