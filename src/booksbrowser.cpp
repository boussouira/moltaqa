#include "booksbrowser.h"
#include "ui_booksbrowser.h"

BooksBrowser::BooksBrowser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BooksBrowser)
{
    ui->setupUi(this);
}

BooksBrowser::~BooksBrowser()
{
    delete ui;
}
