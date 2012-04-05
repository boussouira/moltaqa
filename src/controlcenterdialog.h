#ifndef CONTROLCENTERDIALOG_H
#define CONTROLCENTERDIALOG_H

#include <qdialog.h>

namespace Ui {
    class ControlCenterDialog;
}

class ControlCenterWidget;

class ControlCenterDialog : public QDialog
{
    Q_OBJECT

public:
    ControlCenterDialog(QWidget *parent = 0);
    ~ControlCenterDialog();

    void addControlWidget(ControlCenterWidget *w);

protected:
    void closeEvent(QCloseEvent *event);
    ControlCenterWidget *currentControlWidget();

private slots:
    void rowChanged(int row);
    void setCurrentRow(int row);
    void save();
    void aboutToShow();
    void aboutToHide();
    void enableSave(bool enable);

private:
    Ui::ControlCenterDialog *ui;
};

#endif // CONTROLCENTERDIALOG_H
