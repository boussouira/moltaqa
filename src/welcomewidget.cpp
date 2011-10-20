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
