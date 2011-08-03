#include "importdialog.h"
#include "ui_importdialog.h"
#include "importmodel.h"
#include "importdelegates.h"
#include "indexdb.h"
#include "convertthread.h"
#include "importthread.h"

#ifdef USE_MDBTOOLS
    #include "mdbconverter.h"
#endif

#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qstandarditemmodel.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsqlrecord.h>
#include <qsqlerror.h>
#include <qsignalmapper.h>
#include <qtoolbutton.h>
#include <qevent.h>
#include <qurl.h>

ImportDialog::ImportDialog(IndexDB *indexDB, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportDialog)
{
    ui->setupUi(this);
    m_model = new ImportModel(ui->treeView);
    m_indexDB = indexDB;

    setAcceptDrops(true);

    m_signalMapper = new QSignalMapper(this);
    connect(m_signalMapper, SIGNAL(mapped(int)), this, SIGNAL(openBook(int)));

    ImportModelNode *node = new ImportModelNode(BookInfo::NormalBook);
    m_model->setRootNode(node);

    ui->treeView->setItemDelegateForColumn(2, new BookTypeDelegate(ui->treeView));
    ui->treeView->setItemDelegateForColumn(3,
                                           new CategorieDelegate(ui->treeView,
                                                                 m_indexDB->booksList(true)));
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
    QStringList files = QFileDialog::getOpenFileNames(this,
                                                      tr("اختر الكتب التي تريد استيرادها:"),
                                                      QString(),
                                                      "Shamela books (*.bok)");
    foreach(QString file, files) {
        addFile(file);
    }
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
                              tr("خطأ عند الاستيراد"),
                              tr("لم تقم باختيار أي ملف ليتم استيراده"));
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
    QString importBooks = arPlural(m_model->nodeFromIndex(QModelIndex())->childs().count(),
                                   0);

    ui->label_2->setText(QString(ui->label_2->text())
                         .arg(convertedFiles)
                         .arg(convertTime)
                         .arg(importBooks));

    ui->stackedWidget->setCurrentIndex(1);
}

void ImportDialog::importBooks()
{
   if(checkNodes(m_model->nodeFromIndex(QModelIndex())->childs())){

        ImportThread *thread = new ImportThread(this);

        thread->setModel(m_model);
        thread->setIndexDB(m_indexDB);
        thread->setSignalMapper(m_signalMapper);

        connect(thread, SIGNAL(finished()), this, SLOT(doneImporting()));
        connect(thread, SIGNAL(setProgress(int)), ui->progressBar, SLOT(setValue(int)));

        setEnabled(false);

        ui->progressBar->setMaximum(m_model->nodeFromIndex(QModelIndex())->childs().count());
        ui->progressBar->setValue(0);
        ui->progressBar->show();

        thread->start();

    } else {
        QMessageBox::warning(this,
                             tr("خطأ عند الاستيراد"),
                             tr("لم تقم باختيار أقسام بعض الكتب"));
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
        button->setToolTip(tr("فتح كتاب %1").arg(i.value()));

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
    ui->pushNext->setText(tr("انتهى"));

    ui->stackedWidget->setCurrentIndex(2);

    emit bookAdded();
}

bool ImportDialog::checkNodes(QList<ImportModelNode *> nodesList)
{
    int wrongNodes = 0;
    foreach(ImportModelNode *node, nodesList) {
        if(node->catID() == -1){
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
        list <<  tr("كتاب واحد") << tr("كتابين") << tr("كتب") << tr("كتابا");
    else if(word==1)
        list <<  tr("ثانية") << tr("ثانيتين") << tr("ثوان") << tr("ثانية");
    else if(word==2)
        list <<  tr("ملف واحد") << tr("ملفين") << tr("ملفات") << tr("ملفا");

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

void ImportDialog::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

void ImportDialog::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasUrls()) {
        foreach(QUrl url, mimeData->urls()){
            QString path = url.toLocalFile();
            QFileInfo info(path);

            if(info.isFile())
                addFile(path);
            else if(info.isDir())
                addDir(path);
        }
    } else {
        qWarning("MimeType is not handled");
    }

    event->acceptProposedAction();
}

void ImportDialog::addFile(const QString &path)
{
    QFileInfo info(path);
    if(info.isFile() && info.suffix() == "bok") {
        if(!fileExsistInList(path))
            ui->fileListWidget->addItem(path);
    }
}

void ImportDialog::addDir(const QString &path)
{
    QDir dir(path);
    qDebug("[*] %s", qPrintable(path));
    foreach(QString file, dir.entryList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot)) {
        QFileInfo info(dir.absoluteFilePath(file));
        if(info.isFile())
            addFile(info.absoluteFilePath());
        else if(info.isDir())
            addDir(info.absoluteFilePath());
    }
}

bool ImportDialog::fileExsistInList(const QString &path)
{
    for(int i=0;i<ui->fileListWidget->count();i++){
        if(ui->fileListWidget->item(i)->text().compare(path, Qt::CaseInsensitive) == 0) {
            return true;
        }
    }

    return false;
}
