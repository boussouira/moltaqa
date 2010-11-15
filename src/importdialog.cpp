#include "importdialog.h"
#include "ui_importdialog.h"
#include "importmodel.h"
#include "importdelegates.h"
#include "booksindexdb.h"
#include "mdbconverter.h"
#include "convertthread.h"
#include "importthread.h"

#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qstandarditemmodel.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsqlrecord.h>
#include <qsqlerror.h>
#include <qsignalmapper.h>
#include <qtoolbutton.h>

ImportDialog::ImportDialog(BooksIndexDB *indexDB, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportDialog)
{
    ui->setupUi(this);
    m_model = new ImportModel(ui->treeView);
    m_indexDB = indexDB;

    m_signalMapper = new QSignalMapper(this);
    connect(m_signalMapper, SIGNAL(mapped(int)), this, SIGNAL(openBook(int)));

    ImportModelNode *node = new ImportModelNode(BookInfo::NormalBook);
    m_model->setRootNode(node);

    ui->treeView->setItemDelegateForColumn(2, new BookTypeDelegate(ui->treeView));
    ui->treeView->setItemDelegateForColumn(3,
                                           new CategorieDelegate(ui->treeView,
                                                                 m_indexDB->getListModel(false)));
    ui->treeView->setModel(m_model);
    ui->progressBar->hide();
}

ImportDialog::~ImportDialog()
{
    delete ui;
}

void ImportDialog::on_pushCancel_clicked()
{
    reject();
}

void ImportDialog::on_pushAddFile_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setNameFilter(tr("Shamela book (*.bok);; All files (*.*)"));
    dialog.setViewMode(QFileDialog::Detail);

    if(dialog.exec())
        ui->fileListWidget->addItems(dialog.selectedFiles());
}

void ImportDialog::on_pushDeleteFile_clicked()
{
    foreach(QModelIndex index, ui->fileListWidget->selectionModel()->selectedIndexes())
        ui->fileListWidget->takeItem(index.row());
}

void ImportDialog::on_pushNext_clicked()
{
    switch(ui->stackedWidget->currentIndex()){
    case 0:
        convertBooks();
        break;
    case 1:
        importBooks();
        break;
    case 2:
        done(QDialog::Accepted);
        break;
    }
}

void ImportDialog::convertBooks()
{
    if(ui->fileListWidget->count()==0) {
        QMessageBox::warning(this,
                              trUtf8("خطأ عند الاستيراد"),
                              trUtf8("لم تقم باختيار أي ملف ليتم استيراده"));
        return;
    }

    QStringList files;
    for(int i=0;i<ui->fileListWidget->count();i++)
        files.append(ui->fileListWidget->item(i)->text());

    ConvertThread *thread = new ConvertThread(this);
    thread->setFiles(files);
    thread->setModel(m_model);
    thread->setIndexDB(m_indexDB);

    connect(thread, SIGNAL(finished()), this, SLOT(doneConverting()));
    connect(thread, SIGNAL(setProgress(int)), ui->progressBar, SLOT(setValue(int)));

    setEnabled(false);

    ui->progressBar->setMaximum(files.count());
    ui->progressBar->setValue(0);
    ui->progressBar->show();

    thread->start();

}


void ImportDialog::doneConverting()
{
    ConvertThread *thread = static_cast<ConvertThread *>(sender());

    setEnabled(true);
    ui->progressBar->hide();

    QString convertedFiles = arPlural(thread->convertedFiles(), 2);
    QString convertTime = arPlural(thread->convertTime()/1000, 1);
    QString importBooks = arPlural(m_model->nodeFromIndex(QModelIndex())->childrenList().count(),
                                   0);

    ui->label_2->setText(QString(ui->label_2->text())
                         .arg(convertedFiles)
                         .arg(convertTime)
                         .arg(importBooks));

    ui->stackedWidget->setCurrentIndex(1);
}

void ImportDialog::importBooks()
{
   if(checkNodes(m_model->nodeFromIndex(QModelIndex())->childrenList())){

        ImportThread *thread = new ImportThread(this);

        thread->setModel(m_model);
        thread->setIndexDB(m_indexDB);
        thread->setSignalMapper(m_signalMapper);

        connect(thread, SIGNAL(finished()), this, SLOT(doneImporting()));
        connect(thread, SIGNAL(setProgress(int)), ui->progressBar, SLOT(setValue(int)));

        setEnabled(false);

        ui->progressBar->setMaximum(m_model->nodeFromIndex(QModelIndex())->childrenList().count());
        ui->progressBar->setValue(0);
        ui->progressBar->show();

        thread->start();

    } else {
        QMessageBox::warning(this,
                             trUtf8("خطأ عند الاستيراد"),
                             trUtf8("لم تقم باختيار أقسام بعض الكتب"));
    }
}

void ImportDialog::doneImporting()
{
    ImportThread *thread = static_cast<ImportThread *>(sender());
    QHash<int, QString>  booksList = thread->booksList();

    ui->progressBar->setMaximum(0);

    QWidget *widget = new QWidget(this);
    QGridLayout *gridLayout = new QGridLayout(widget);

    widget->setLayout(gridLayout);
    ui->scrollArea->setWidget(widget);

    QHashIterator<int, QString> i(booksList);
    while(i.hasNext()){
        i.next();
        QToolButton *button = new QToolButton;
        button->setMaximumSize(40,40);
        button->setIcon(QIcon(":/menu/images/go-previous.png"));
        button->setStyleSheet("padding:5px;");
        button->setToolTip(trUtf8("فتح كتاب %1").arg(i.value()));

        QLabel *label = new QLabel(i.value());
        label->setStyleSheet("padding:5px;border:1px solid #cccccc;");

        int row = gridLayout->rowCount();
        gridLayout->addWidget(label, row, 0);
        gridLayout->addWidget(button, row, 1);

        connect(button, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
        m_signalMapper->setMapping(button, i.key());
    }

    setEnabled(true);
    setModal(false);
    ui->pushCancel->hide();
    ui->progressBar->hide();
    ui->pushNext->setText(trUtf8("انتهى"));

    ui->stackedWidget->setCurrentIndex(2);
}

bool ImportDialog::checkNodes(QList<ImportModelNode *> nodesList)
{
    int wrongNodes = 0;
    foreach(ImportModelNode *node, nodesList) {
        if(node->getCatID() == -1){
            node->setBackgroundColor(QColor(200,200,200));
            wrongNodes++;
        } else {
            node->setBackgroundColor(Qt::white);
        }
    }

    return (!wrongNodes);
}

QString ImportDialog::arPlural(int count, int word)
{
    QStringList list;
    if(word==0)
        list <<  trUtf8("كتاب واحد") << trUtf8("كتابين") << trUtf8("كتب") << trUtf8("كتابا");
    else if(word==1)
        list <<  trUtf8("ثانية") << trUtf8("ثانيتين") << trUtf8("ثوان") << trUtf8("ثانية");
    else if(word==2)
        list <<  trUtf8("ملف واحد") << trUtf8("ملفين") << trUtf8("ملفات") << trUtf8("ملفا");

    if(count == 1){
        return list.at(0);
    } else if(count == 2) {
        return list.at(1);
    } else if (count > 2 && count <= 10) {
        return QString("%1 %2").arg(count).arg(list.at(2));
    } else if (count > 10) {
        return QString("%1 %2").arg(count).arg(list.at(3));
    } else {
        return QString();
    }
}
