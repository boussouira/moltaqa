#ifndef WELCOMEWIDGET_H
#define WELCOMEWIDGET_H

#include "abstarctview.h"

namespace Ui {
    class WelcomeWidget;
}

class WelcomeWidget : public AbstarctView
{
    Q_OBJECT

public:
    WelcomeWidget(QWidget *parent = 0);
    ~WelcomeWidget();

    QString title();

private slots:
    void on_pushBooksList_clicked();
    void on_webView_loadStarted();
    void on_webView_loadFinished(bool);
    void on_webView_loadProgress(int progress);

signals:
    void showBooksList();

private:
    Ui::WelcomeWidget *ui;
};

#endif // WELCOMEWIDGET_H
