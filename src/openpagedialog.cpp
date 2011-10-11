#include "openpagedialog.h"
#include "ui_openpagedialog.h"

OpenPageDialog::OpenPageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenPageDialog)
{
    ui->setupUi(this);

    m_pageNum = -1;
    m_partNum = -1;
    m_info = 0;

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

void OpenPageDialog::setBookInfo(BookInfo *info)
{
    m_info = info;

    ui->spinPage->setValue(info->currentPage.page);
    ui->spinPart->setValue(info->currentPage.part);

    ui->spinPart->setMaximum(info->partsCount);

    if(info->isQuran()) {
        ui->spinPage->setMaximum(info->lastPage());
    }

    if(info->isQuran() || info->isTafessir()) {
        ui->spinAya->setValue(info->currentPage.aya);
        ui->comboSora->setCurrentIndex(qMax(0, info->currentPage.sora-1));
    } else {
        ui->tab_2->setEnabled(false);
    }

    if(!info->isQuran())
        ui->spinHaddit->setValue(info->currentPage.haddit);
}

void OpenPageDialog::loadSowarNames()
{
    // TODO: optimize this?
    ui->comboSora->addItem(tr("الفاتحة"),
                           7);
    ui->comboSora->addItem(tr("البقرة"),
                           286);
    ui->comboSora->addItem(tr("آل عمران"),
                           200);
    ui->comboSora->addItem(tr("النساء"),
                           176);
    ui->comboSora->addItem(tr("المائدة"),
                           120);
    ui->comboSora->addItem(tr("الأنعام"),
                           165);
    ui->comboSora->addItem(tr("الأعراف"),
                           206);
    ui->comboSora->addItem(tr("الأنفال"),
                           75);
    ui->comboSora->addItem(tr("التوبة"),
                           129);
    ui->comboSora->addItem(tr("يونس"),
                           109);
    ui->comboSora->addItem(tr("هود"),
                           123);
    ui->comboSora->addItem(tr("يوسف"),
                           111);
    ui->comboSora->addItem(tr("الرعد"),
                           43);
    ui->comboSora->addItem(tr("إبراهيم"),
                           52);
    ui->comboSora->addItem(tr("الحجر"),
                           99);
    ui->comboSora->addItem(tr("النحل"),
                           128);
    ui->comboSora->addItem(tr("الإسراء"),
                           111);
    ui->comboSora->addItem(tr("الكهف"),
                           110);
    ui->comboSora->addItem(tr("مريم"),
                           98);
    ui->comboSora->addItem(tr("طه"),
                           135);
    ui->comboSora->addItem(tr("الأنبياء"),
                           112);
    ui->comboSora->addItem(tr("الحج"),
                           78);
    ui->comboSora->addItem(tr("المؤمنون"),
                           118);
    ui->comboSora->addItem(tr("النور"),
                           64);
    ui->comboSora->addItem(tr("الفرقان"),
                           77);
    ui->comboSora->addItem(tr("الشعراء"),
                           227);
    ui->comboSora->addItem(tr("النمل"),
                           93);
    ui->comboSora->addItem(tr("القصص"),
                           88);
    ui->comboSora->addItem(tr("العنكبوت"),
                           69);
    ui->comboSora->addItem(tr("الروم"),
                           60);
    ui->comboSora->addItem(tr("لقمان"),
                           34);
    ui->comboSora->addItem(tr("السجدة"),
                           30);
    ui->comboSora->addItem(tr("الأحزاب"),
                           73);
    ui->comboSora->addItem(tr("سبأ"),
                           54);
    ui->comboSora->addItem(tr("فاطر"),
                           45);
    ui->comboSora->addItem(tr("يس"),
                           83);
    ui->comboSora->addItem(tr("الصافات"),
                           182);
    ui->comboSora->addItem(tr("ص"),
                           88);
    ui->comboSora->addItem(tr("الزمر"),
                           75);
    ui->comboSora->addItem(tr("غافر"),
                           85);
    ui->comboSora->addItem(tr("فصلت"),
                           54);
    ui->comboSora->addItem(tr("الشورى"),
                           53);
    ui->comboSora->addItem(tr("الزخرف"),
                           89);
    ui->comboSora->addItem(tr("الدخان"),
                           59);
    ui->comboSora->addItem(tr("الجاثية"),
                           37);
    ui->comboSora->addItem(tr("الأحقاف"),
                           35);
    ui->comboSora->addItem(tr("محمد"),
                           38);
    ui->comboSora->addItem(tr("الفتح"),
                           29);
    ui->comboSora->addItem(tr("الحجرات"),
                           18);
    ui->comboSora->addItem(tr("ق"),
                           45);
    ui->comboSora->addItem(tr("الذاريات"),
                           60);
    ui->comboSora->addItem(tr("الطور"),
                           49);
    ui->comboSora->addItem(tr("النجم"),
                           62);
    ui->comboSora->addItem(tr("القمر"),
                           55);
    ui->comboSora->addItem(tr("الرحمن"),
                           78);
    ui->comboSora->addItem(tr("الواقعة"),
                           96);
    ui->comboSora->addItem(tr("الحديد"),
                           29);
    ui->comboSora->addItem(tr("المجادلة"),
                           22);
    ui->comboSora->addItem(tr("الحشر"),
                           24);
    ui->comboSora->addItem(tr("الممتحنة"),
                           13);
    ui->comboSora->addItem(tr("الصف"),
                           14);
    ui->comboSora->addItem(tr("الجمعة"),
                           11);
    ui->comboSora->addItem(tr("المنافقون"),
                           11);
    ui->comboSora->addItem(tr("التغابن"),
                           18);
    ui->comboSora->addItem(tr("الطلاق"),
                           12);
    ui->comboSora->addItem(tr("التحريم"),
                           12);
    ui->comboSora->addItem(tr("الملك"),
                           30);
    ui->comboSora->addItem(tr("القلم"),
                           52);
    ui->comboSora->addItem(tr("الحاقة"),
                           52);
    ui->comboSora->addItem(tr("المعارج"),
                           44);
    ui->comboSora->addItem(tr("نوح"),
                           28);
    ui->comboSora->addItem(tr("الجن"),
                           28);
    ui->comboSora->addItem(tr("المزمل"),
                           20);
    ui->comboSora->addItem(tr("المدثر"),
                           56);
    ui->comboSora->addItem(tr("القيامة"),
                           40);
    ui->comboSora->addItem(tr("الإنسان"),
                           31);
    ui->comboSora->addItem(tr("المرسلات"),
                           50);
    ui->comboSora->addItem(tr("النبأ"),
                           40);
    ui->comboSora->addItem(tr("النازعات"),
                           46);
    ui->comboSora->addItem(tr("عبس"),
                           42);
    ui->comboSora->addItem(tr("التكوير"),
                           29);
    ui->comboSora->addItem(tr("الإنفطار"),
                           19);
    ui->comboSora->addItem(tr("المطففين"),
                           36);
    ui->comboSora->addItem(tr("الإنشقاق"),
                           25);
    ui->comboSora->addItem(tr("البروج"),
                           22);
    ui->comboSora->addItem(tr("الطارق"),
                           17);
    ui->comboSora->addItem(tr("الأعلى"),
                           19);
    ui->comboSora->addItem(tr("الغاشية"),
                           26);
    ui->comboSora->addItem(tr("الفجر"),
                           30);
    ui->comboSora->addItem(tr("البلد"),
                           20);
    ui->comboSora->addItem(tr("الشمس"),
                           15);
    ui->comboSora->addItem(tr("الليل"),
                           21);
    ui->comboSora->addItem(tr("الضحى"),
                           11);
    ui->comboSora->addItem(tr("الشرح"),
                           8);
    ui->comboSora->addItem(tr("التين"),
                           8);
    ui->comboSora->addItem(tr("العلق"),
                           19);
    ui->comboSora->addItem(tr("القدر"),
                           5);
    ui->comboSora->addItem(tr("البينة"),
                           8);
    ui->comboSora->addItem(tr("الزلزلة"),
                           8);
    ui->comboSora->addItem(tr("العاديات"),
                           11);
    ui->comboSora->addItem(tr("القارعة"),
                           11);
    ui->comboSora->addItem(tr("التكاثر"),
                           8);
    ui->comboSora->addItem(tr("العصر"),
                           3);
    ui->comboSora->addItem(tr("الهمزة"),
                           9);
    ui->comboSora->addItem(tr("الفيل"),
                           5);
    ui->comboSora->addItem(tr("قريش"),
                           4);
    ui->comboSora->addItem(tr("الماعون"),
                           7);
    ui->comboSora->addItem(tr("الكوثر"),
                           3);
    ui->comboSora->addItem(tr("الكافرون"),
                           6);
    ui->comboSora->addItem(tr("النصر"),
                           3);
    ui->comboSora->addItem(tr("المسد"),
                           5);
    ui->comboSora->addItem(tr("الإخلاص"),
                           4);
    ui->comboSora->addItem(tr("الفلق"),
                           5);
    ui->comboSora->addItem(tr("الناس"),
                           6);
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
    /*
    if(!m_info)
        return;

    if(ui->spinPart->value() < m_info->partsCount) {
        ui->spinPage->setMaximum(m_info->lastPage(ui->spinPart->value()));
    }
    */
}
