#ifndef CONTROLCENTERDIALOG_H
#define CONTROLCENTERDIALOG_H

#include <qdialog.h>

namespace Ui {
    class ControlCenterDialog;
}

class AbstractEditWidget;

class ControlCenterDialog : public QDialog
{
    Q_OBJECT

public:
    ControlCenterDialog(QWidget *parent = 0);
    ~ControlCenterDialog();

    void addEditWidget(int index, AbstractEditWidget *w);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void rowChanged(int row);
    void save();
    void beginEdit();
    void enableSave(bool enable);

private:
    Ui::ControlCenterDialog *ui;
};

#endif // CONTROLCENTERDIALOG_H
