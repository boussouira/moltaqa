#ifndef WELCOMEWIDGET_H
#define WELCOMEWIDGET_H

#include <QWidget>

namespace Ui {
    class WelcomeWidget;
}

class WelcomeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WelcomeWidget(QWidget *parent = 0);
    ~WelcomeWidget();

private slots:
    void on_pushBooksList_clicked();

signals:
    void showBooksList();

private:
    Ui::WelcomeWidget *ui;
};

#endif // WELCOMEWIDGET_H