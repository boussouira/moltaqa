#include "shamelaimportdialog.h"
#include "ui_shamelaimportdialog.h"
#include "shamelainfo.h"
#include "shamelamanager.h"
#include "common.h"
#include "shamelaimportthread.h"

#include <qdir.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qfuture.h>
#include <qtconcurrentrun.h>
#include <qstandarditemmodel.h>
#include <qsortfilterproxymodel.h>
#include <qmessagebox.h>

ShamelaImportDialog::ShamelaImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShamelaImportDialog)
{
    ui->setupUi(this);

    m_shamela = new ShamelaInfo();
    m_manager = new ShamelaManager(m_shamela);

    ui->groupImportOptions->setEnabled(false);
    ui->stackedWidget->setCurrentIndex(0);
    ui->pushDone->hide();

    connect(ui->pushNext, SIGNAL(clicked()), SLOT(nextStep()));
    connect(ui->pushCancel, SIGNAL(clicked()), SLOT(cancel()));
    connect(ui->pushDone, SIGNAL(clicked()), SLOT(accept()));
    connect(this, SIGNAL(setProgress(int)), ui->progressBar, SLOT(setValue(int)));
}

ShamelaImportDialog::~ShamelaImportDialog()
{
    delete m_shamela;
    delete m_manager;
    delete ui;
}

QString ShamelaImportDialog::getFolderPath(const QString &defaultPath)
{
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("اختر مجلد"),
                                                        defaultPath,
                                                        QFileDialog::ShowDirsOnly
                                                        |QFileDialog::DontResolveSymlinks);

    if(!dirPath.isEmpty()) {
        QDir dir(dirPath);
        return dir.absolutePath();
    } else {
        return QString();
    }
}

void ShamelaImportDialog::on_buttonSelectShamela_clicked()
{
    QString path = getFolderPath(ui->lineShamelaDir->text());

    if(!path.isEmpty()){
        if(m_shamela->isShamelaPath(path)) {
            ui->lineShamelaDir->setText(QDir::toNativeSeparators(path));
            ui->groupImportOptions->setEnabled(true);

            m_shamela->setShamelaPath(path);
        } else {
            QMessageBox::warning(this,
                                 tr("الاستيراد من الشاملة"),
                                 tr("لم تقم باختيار مسار المكتبة الشاملة بشكل صحيح"));
        }
    }
}

void ShamelaImportDialog::nextStep()
{
    int index = ui->stackedWidget->currentIndex();

    if(index == 0) {
        if(!m_shamela->shamelaPath().isEmpty()) {
            if(ui->radioImportAll->isChecked()) {
                showImportInfo();
                goPage(2);
            } else {
                showBooks();
                goPage();
            }
        } else {
            QMessageBox::warning(this,
                                 tr("الاستيراد من الشاملة"),
                                 tr("لم تقم باختيار مسار المكتبة الشاملة"));
        }
    } else if(index == 1) {
        createFilter();
        showImportInfo();
        goPage();
    } else if(index == 2) { // Start importing
        goPage();
        startImporting();
        ui->pushNext->setEnabled(false);
    }
}

void ShamelaImportDialog::goPage(int index)
{
    int currentIndex = (index == -1) ? ui->stackedWidget->currentIndex()+1 : index;

    ui->stackedWidget->setCurrentIndex(currentIndex);
}

void ShamelaImportDialog::showBooks()
{
    m_booksModel = new QStandardItemModel(this);
    QSortFilterProxyModel *filterModel = new QSortFilterProxyModel(this);

    m_manager->selectBooks();

    ShamelaBookInfo *book = m_manager->nextBook();
    while(book) {
        QStandardItem *item = new QStandardItem(book->name);
        item->setData(book->id);
        item->setCheckable(true);
        item->setCheckState(Qt::Unchecked);
        m_booksModel->appendRow(item);

        book = m_manager->nextBook();
    }
    filterModel->setSourceModel(m_booksModel);
    ui->treeView->setModel(filterModel);

    m_booksModel->setHeaderData(0, Qt::Horizontal, tr("لائحة الكتب"), Qt::DisplayRole);

    connect(ui->lineBookSearch, SIGNAL(textChanged(QString)), filterModel, SLOT(setFilterRegExp(QString)));
}

void ShamelaImportDialog::createFilter()
{
    QList<int> selectedIDs;

    for(int i=0; i<m_booksModel->rowCount(); i++) {
        QStandardItem *item = m_booksModel->item(i);
        if(item->checkState() == Qt::Checked){
            selectedIDs.append(item->data().toInt());
        }
    }

    m_manager->setFilterBooks(true);

    if(ui->radioImportSelected->isChecked())
        m_manager->setAcceptedBooks(selectedIDs);
    else
        m_manager->setRejectedBooks(selectedIDs);
}

void ShamelaImportDialog::showImportInfo()
{
    int booksCount = m_manager->getBooksCount();
    int authorsCount = m_manager->getAuthorsCount();
    int catCount = m_manager->getCatCount();

    QString info = tr("<ul>"
                      "<li>%1</li>"
                      "<li>%2</li>"
                      "<li>%3</li>"
                      "</ul>")
            .arg(arPlural(booksCount, BOOK))
            .arg(arPlural(authorsCount, AUTHOR))
            .arg(arPlural(catCount, CATEGORIE));

    ui->labelImportInfo->setText(info);
}

void ShamelaImportDialog::startImporting()
{
    m_importThread = new ShamelaImportThread(this);
    m_importThread->m_manager = m_manager;
    m_importThread->m_shamela = m_shamela;
    m_importThread->m_library = m_library;

    connect(m_importThread, SIGNAL(setStepsRange(int,int)), ui->progressSteps, SLOT(setRange(int,int)));
    connect(m_importThread, SIGNAL(setStep(int)), ui->progressSteps, SLOT(setValue(int)));

    connect(m_importThread, SIGNAL(setRange(int,int)), ui->progressBar, SLOT(setRange(int,int)));
    connect(m_importThread, SIGNAL(setProgress(int)), ui->progressBar, SLOT(setValue(int)));

    connect(m_importThread, SIGNAL(stepTitle(QString)), SLOT(setStepTitle(QString)));
    connect(m_importThread, SIGNAL(debugInfo(QString)), SLOT(addDebugInfo(QString)));
    connect(m_importThread, SIGNAL(doneImporting()), SLOT(doneImporting()));

    m_importThread->start();
}

void ShamelaImportDialog::setStepTitle(const QString &title)
{
    ui->groupStep->setTitle(title);
}

void ShamelaImportDialog::addDebugInfo(const QString &text)
{
    ui->listDebug->addItem(text);
    ui->listDebug->scrollToBottom();
}

void ShamelaImportDialog::doneImporting()
{
    ui->pushNext->hide();
    ui->pushCancel->hide();
    ui->pushDone->show();
}

void ShamelaImportDialog::cancel()
{
    if(m_importThread->isRunning()){
        int rep = QMessageBox::question(this,
                                        tr("ايقاف الاستيراد"),
                                        tr("هل تريد ايقاف استيراد الكتب؟"),
                                        QMessageBox::Yes|QMessageBox::No);
        if(rep == QMessageBox::Yes)
            m_importThread->stop();
    } else {
        reject();
    }
}