#include "importdialog.h"
#include "ui_importdialog.h"
#include "importmodel.h"
#include "importdelegates.h"
#include "librarymanager.h"
#include "librarybookmanager.h"
#include "convertthread.h"
#include "utils.h"
#include "stringutils.h"
#include "timeutils.h"
#include "mainwindow.h"
#include "indextracker.h"
#include "booklistmanager.h"
#include "taffesirlistmanager.h"

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
#include <QTime>
#include <qtconcurrentrun.h>
#include <qsettings.h>

ImportDialog::ImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportDialog)
{
    ui->setupUi(this);

    m_model = new ImportModel(ui->treeView);
    m_libraryManager = LibraryManager::instance();

    setAcceptDrops(true);

    m_signalMapper = new QSignalMapper(this);
    connect(m_signalMapper, SIGNAL(mapped(int)), this, SIGNAL(openBook(int)));

    ImportModelNode *node = new ImportModelNode(LibraryBook::NormalBook);
    m_model->setRootNode(node);

    m_authorDelegate = new AuthorDelegate(this);
    m_bookTypeDelegate = new BookTypeDelegate(this);
    m_categorieDelegate = new CategorieDelegate(this);

    ui->treeView->setItemDelegateForColumn(1, m_authorDelegate);
    ui->treeView->setItemDelegateForColumn(2, m_bookTypeDelegate);
    ui->treeView->setItemDelegateForColumn(3, m_categorieDelegate);
    ui->treeView->setModel(m_model);
    ui->progressBar->hide();

    Utils::Widget::restorePosition(this, "ImportDialog");
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
    QSettings settings;
    QString lastPath = settings.value("SavedPath/ImportDialog").toString();

    QStringList files = QFileDialog::getOpenFileNames(this,
                                                      tr("اختر الكتب التي تريد استيرادها:"),
                                                      lastPath,
                                                      "Shamela books (*.bok)");
    foreach(QString file, files) {
        addFile(file);
    }

    if(!files.isEmpty())
        settings.setValue("SavedPath/ImportDialog",
                          QFileInfo(files.first()).absolutePath());
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
        files.append(ui->fileListWidget->item(i)->toolTip());

    ConvertThread *thread = new ConvertThread(this);
    thread->setFiles(files);
    thread->setModel(m_model);
    thread->setLibraryManager(m_libraryManager);

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

    QString convertedFiles = Utils::String::Arabic::plural(thread->convertedFiles(), Utils::String::Arabic::FILES);
    QString convertTime = Utils::Time::secondsToString(thread->convertTime(), true);
    QString importBooks = Utils::String::Arabic::plural(m_model->nodeFromIndex()->childrenNode.count(),
                                   Utils::String::Arabic::BOOK);

    ui->labelConvertInfo->setText(tr("تم تحويل %1 خلال %2،" "<br>"
                                     "سيتم استيراد %3:")
                         .arg(convertedFiles)
                         .arg(convertTime)
                         .arg(importBooks));

    ui->stackedWidget->setCurrentIndex(1);
}

void ImportDialog::importBooks()
{
    if(!checkNodes(m_model->nodeFromIndex()->childrenNode)) {
        int rep = QMessageBox::question(this,
                                        tr("خطأ عند الاستيراد"),
                                        tr("لم تقم باختيار أقسام بعض الكتب" "\n"
                                           "هل تريد المتابعة؟"),
                                        QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
        ML_ASSERT(rep != QMessageBox::No);
    }

    setEnabled(false);

    ui->progressBar->setMaximum(m_model->nodeFromIndex()->childrenNode.count());
    ui->progressBar->setValue(0);
    ui->progressBar->show();

    connect(&m_importWatcher, SIGNAL(finished()), SLOT(doneImporting()));

    QFuture<void> future = QtConcurrent::run(this, &ImportDialog::startImporting);
    m_importWatcher.setFuture(future);
}

void ImportDialog::startImporting()
{
    QTime time;
    time.start();

    QList<ImportModelNode *> nodesList = m_model->nodeFromIndex()->childrenNode;
    int imported = 0;

    for(int i=0;i<nodesList.count();i++) {
        ImportModelNode *node = nodesList.at(i);
        int lastInsert = m_libraryManager->addBook(node);

        if(lastInsert != -1) {
            imported++;
            m_booksList.insert(lastInsert, node->bookName);
        } else {
            qWarning() << "Error:" << node->bookName;
        }

        metaObject()->invokeMethod(ui->progressBar, "setValue",
                                   Q_ARG(int, i+1));
    }

    qDebug() << "Importing" << imported << "books take" << time.elapsed() << "ms";
}

void ImportDialog::doneImporting()
{
    ui->progressBar->setMaximum(0);

    QWidget *widget = new QWidget(this);
    QGridLayout *gridLayout = new QGridLayout(widget);

    widget->setLayout(gridLayout);
    ui->scrollArea->setWidget(widget);

    QHashIterator<int, QString> i(m_booksList);
    while(i.hasNext()){
        i.next();
        QToolButton *button = new QToolButton;
        button->setMaximumSize(40,40);
        button->setIcon(QIcon(":/images/go-previous.png"));
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

    IndexTracker::instance()->addTask(m_booksList.keys(), IndexTask::Add);
    IndexTracker::instance()->save();

    setEnabled(true);
    setModal(false);
    ui->pushCancel->hide();
    ui->progressBar->hide();
    ui->pushNext->setText(tr("انتهى"));

    ui->stackedWidget->setCurrentIndex(2);

    m_libraryManager->reloadManagers();
}

bool ImportDialog::checkNodes(QList<ImportModelNode *> nodesList)
{
    int wrongNodes = 0;
    foreach(ImportModelNode *node, nodesList) {
        if(node->catID == 0){
            node->bgColor = Qt::lightGray;
            wrongNodes++;
        } else {
            node->bgColor = Qt::white;
        }
    }

    return (!wrongNodes);
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

void ImportDialog::closeEvent(QCloseEvent *event)
{
    Utils::Widget::savePosition(this, "ImportDialog");
    event->accept();
}

void ImportDialog::addFile(const QString &path)
{
    QFileInfo info(path);
    if(info.isFile() && info.suffix().compare("bok", Qt::CaseInsensitive)==0) {
        if(!fileExsistInList(path)) {
            QListWidgetItem *item = new QListWidgetItem(ui->fileListWidget);
            item->setText(info.baseName());
            item->setToolTip(path);

            ui->fileListWidget->addItem(item);
        }
    }
}

void ImportDialog::addDir(const QString &path)
{
    QDir dir(path);
    qDebug() << "Feteching directory" << path;
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
        if(ui->fileListWidget->item(i)->toolTip().compare(path, Qt::CaseInsensitive) == 0) {
            return true;
        }
    }

    return false;
}
