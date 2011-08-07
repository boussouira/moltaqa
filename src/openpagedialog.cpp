#include "openpagedialog.h"
#include "ui_openpagedialog.h"

OpenPageDialog::OpenPageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenPageDialog)
{
    ui->setupUi(this);

    m_pageNum = -1;
    m_partNum = -1;
}

OpenPageDialog::~OpenPageDialog()
{
    delete ui;
}

void OpenPageDialog::on_pushCancel_clicked()
{
    reject();
}

void OpenPageDialog::on_pushGo_clicked()
{
    m_pageNum = ui->spinPage->value();
    m_partNum = ui->spinPart->value();

    accept();
}

int OpenPageDialog::selectedPage()
{
    return m_pageNum;
}

int OpenPageDialog::selectedPart()
{
    return m_partNum;
}

void OpenPageDialog::setPage(int page)
{
    ui->spinPage->setValue(page);
}

void OpenPageDialog::setPart(int part)
{
    ui->spinPart->setValue(part);
}
