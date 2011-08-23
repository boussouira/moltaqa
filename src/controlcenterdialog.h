#ifndef CONTROLCENTERDIALOG_H
#define CONTROLCENTERDIALOG_H

#include <qdialog.h>

namespace Ui {
    class ControlCenterDialog;
}

class ControlCenterDialog : public QDialog
{
    Q_OBJECT

public:
    ControlCenterDialog(QWidget *parent = 0);
    ~ControlCenterDialog();

    void addEditWidget(int index, QWidget *w);

private slots:
    void rowChanged(int row);

private:
    Ui::ControlCenterDialog *ui;
};

#endif // CONTROLCENTERDIALOG_H
