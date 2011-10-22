#include "welcomewidget.h"
#include "ui_welcomewidget.h"

WelcomeWidget::WelcomeWidget(QWidget *parent) :
    AbstarctView(parent),
    ui(new Ui::WelcomeWidget)
{
    ui->setupUi(this);
}

WelcomeWidget::~WelcomeWidget()
{
    delete ui;
}

void WelcomeWidget::on_pushBooksList_clicked()
{
    emit showBooksList();
}

QString WelcomeWidget::title()
{
    return tr("صفحة البداية");
}

void WelcomeWidget::on_webView_loadStarted()
{
    ui->progressBar->setValue(0);
    ui->progressBar->show();
}

void WelcomeWidget::on_webView_loadFinished(bool arg1)
{
    ui->progressBar->hide();
}

void WelcomeWidget::on_webView_loadProgress(int progress)
{
    ui->progressBar->setValue(progress);
}
