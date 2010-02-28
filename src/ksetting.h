#ifndef KSETTING_H
#define KSETTING_H

#include <QWidget>
#include <QDialog>

namespace Ui {
    class KSetting;
}

class KSetting : public QDialog {
    Q_OBJECT
public:
    explicit KSetting(QWidget *parent = 0);
    ~KSetting();

protected:
    void changeEvent(QEvent *e);
    void loadSettings();
    QString getFilePath();
protected slots:
    void changeQuranDBPath();
    void cancel();
    void saveSettings();
private:
    Ui::KSetting *ui;
};

#endif // KSETTING_H
