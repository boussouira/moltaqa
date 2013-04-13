#include "openpagedialog.h"
#include "ui_openpagedialog.h"
#include "bookreaderhelper.h"
#include "mainwindow.h"

OpenPageDialog::OpenPageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenPageDialog)
{
    ui->setupUi(this);

    m_pageNum = -1;
    m_partNum = -1;

    loadSowarNames();
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
    accept();
}

int OpenPageDialog::selectedPage()
{
    return ui->spinPage->value();
}

int OpenPageDialog::selectedPart()
{
    return ui->spinPart->value();
}

void OpenPageDialog::setInfo(LibraryBook::Ptr info, BookPage *page)
{
    m_info = info;

    ui->spinPage->setValue(page->page);
    ui->spinPart->setValue(page->part);

    if(info->isQuran() || info->isTafessir()) {
        ui->comboSora->setCurrentIndex(qMax(0, page->sora-1));
        ui->spinAya->setValue(page->aya);
    } else {
        ui->tab_2->setEnabled(false);
    }

    if(!info->isQuran())
        ui->spinHaddit->setValue(page->haddit);
}

void OpenPageDialog::loadSowarNames()
{
    BookReaderHelper *helper = MW->readerHelper();
    for(int i=1; i<= 114; i++) {
        QuranSora *sora = helper->getQuranSora(i);

        if(sora) {
            ui->comboSora->addItem(sora->name, sora->ayatCount);
        }
    }
}

int OpenPageDialog::selectedSora()
{
    return ui->comboSora->currentIndex()+1;
}

int OpenPageDialog::selectedAya()
{
    return ui->spinAya->value();
}

int OpenPageDialog::selectedHaddit()
{
    return ui->spinHaddit->value();
}

int OpenPageDialog::currentPage()
{
    return ui->tabWidget->currentIndex();
}


void OpenPageDialog::on_comboSora_currentIndexChanged(int index)
{
    ui->spinAya->setMaximum(ui->comboSora->itemData(index).toInt());
}

void OpenPageDialog::on_spinPart_editingFinished()
{
}
