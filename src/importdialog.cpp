#include "importdialog.h"
#include "ui_importdialog.h"
#include "booklistmanager.h"
#include "checkablemessagebox.h"
#include "convertthread.h"
#include "importdelegates.h"
#include "importmodel.h"
#include "indextracker.h"
#include "librarybookmanager.h"
#include "librarymanager.h"
#include "mainwindow.h"
#include "stringutils.h"
#include "taffesirlistmanager.h"
#include "timeutils.h"
#include "utils.h"

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
#include <qtconcurrentrun.h>

ImportDialog::ImportDialog(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::ImportDialog)
{
    ui->setupUi(this);

    setOption(DisabledBackButtonOnLastPage);

    m_acceptedTypes.append("bok");
    m_acceptedTypes.append("mlp");

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

    m_progress.setWindowTitle(tr("استيراد الكتب"));
    m_progress.setWindowModality(Qt::ApplicationModal);
    m_progress.setCancelButton(0);
    m_progress.hide();

    button(NextButton)->disconnect();

    connect(button(NextButton), SIGNAL(clicked()), SLOT(nextPage()));
    connect(ui->pushAddFile, SIGNAL(clicked()), SLOT(selectFiles()));
    connect(ui->pushDeleteFile, SIGNAL(clicked()), SLOT(deleteFiles()));
}

ImportDialog::~ImportDialog()
{
    delete ui;
}

void ImportDialog::addFile(const QString &path)
{
    QFileInfo info(path);
    if(info.isFile() && m_acceptedTypes.contains(info.suffix().toLower())) {
        if(!fileExsistInList(path)) {
            QListWidgetItem *item = new QListWidgetItem(ui->fileListWidget);
            item->setText(QDir::toNativeSeparators(path));
            item->setToolTip(path);

            ui->fileListWidget->addItem(item);
        }
    }
}

void ImportDialog::addDir(const QString &path)
{
    QDir dir(path);
    qDebug() << "ImportDialog::addDir Feteching directory" << path;
    foreach(QString file, dir.entryList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot)) {
        QFileInfo info(dir.absoluteFilePath(file));
        if(info.isFile())
            addFile(info.absoluteFilePath());
        else if(info.isDir())
            addDir(info.absoluteFilePath());
    }
}

bool ImportDialog::validateCurrentPage()
{
    if(currentId() == Page_BookSelection) {
        if(!ui->fileListWidget->count()) {
            QMessageBox::warning(this,
                                 tr("استيراد الكتب"),
                                 tr("لم تقم باختيار أي ملف ليتم استيراده"));
            return false;
        }
    }

    return true;
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
        qWarning("ImportDialog: MimeType is not handled");
    }

    event->acceptProposedAction();
}

void ImportDialog::closeEvent(QCloseEvent *event)
{
    Utils::Widget::save(this, "ImportDialog");
    event->accept();
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

void ImportDialog::convertBooks()
{
    QStringList files;
    for(int i=0;i<ui->fileListWidget->count();i++)
        files.append(ui->fileListWidget->item(i)->toolTip());

    ConvertThread *thread = new ConvertThread(this);
    thread->setFiles(files);
    thread->setModel(m_model);
    thread->setLibraryManager(m_libraryManager);

    connect(thread, SIGNAL(finished()), SLOT(doneConverting()));
    connect(thread, SIGNAL(bookConverted(QString)), SLOT(bookConverted(QString)));
    connect(thread, SIGNAL(addBooksToProgress(int)), SLOT(addBooksToProgress(int)));

    m_progress.setWindowIcon(windowIcon());
    m_progress.setLabelText(tr("جاري تحويل الكتب"));
    m_progress.setMaximum(files.count());
    m_progress.setValue(0);
    m_progress.show();

    thread->start();
}

void ImportDialog::importBooks()
{
    if(!checkNodes(m_model->nodeFromIndex()->childrenNode)) {
        int rep = CheckableMessageBox::question(this,
                                                tr("خطأ عند الاستيراد"),
                                                tr("لم تقم باختيار أقسام بعض الكتب" "\n"
                                                   "هل تريد المتابعة؟"),
                                                "CheckableMessageBox/importBooks",
                                                QDialogButtonBox::Yes);
        if(rep == QDialogButtonBox::No)
            return;
    }

    m_progress.setMaximum(m_model->nodeFromIndex()->childrenNode.count());
    m_progress.setValue(0);
    m_progress.show();

    connect(&m_importWatcher, SIGNAL(finished()), SLOT(doneImporting()));

    QFuture<void> future = QtConcurrent::run(this, &ImportDialog::startImporting);
    m_importWatcher.setFuture(future);
}

void ImportDialog::doneConverting()
{
    ConvertThread *thread = static_cast<ConvertThread *>(sender());

    m_progress.hide();

    QString convertedFiles = Utils::String::Arabic::plural(thread->convertedFiles(), Utils::String::Arabic::FILES);
    QString convertTime = Utils::Time::secondsToString(thread->convertTime(), true);
    QString importBooks = Utils::String::Arabic::plural(m_model->nodeFromIndex()->childrenNode.count(),
                                   Utils::String::Arabic::BOOK);

    ui->labelConvertInfo->setText(tr("تم تحويل %1 خلال %2،" "<br>"
                                     "سيتم استيراد %3:")
                                  .arg(convertedFiles)
                                  .arg(convertTime)
                                  .arg(importBooks));

    next();
}

void ImportDialog::startImporting()
{
    QTime time;
    time.start();

    QList<ImportModelNode *> nodesList = m_model->nodeFromIndex()->childrenNode;
    int imported = 0;

    for(int i=0;i<nodesList.count();i++) {
        try {
            ImportModelNode *node = nodesList.at(i);
            int lastInsert = m_libraryManager->addBook(node);

            if(lastInsert != -1) {
                imported++;
                m_booksList.insert(lastInsert, node->title);
            } else {
                qWarning() << "ImportDialog: Unknow error when importing" << node->title;
            }

            metaObject()->invokeMethod(&m_progress, "setValue",
                                       Q_ARG(int, i+1));
        } catch(BookException &e) {
            e.print();
        }
    }

    qDebug() << "ImportDialog: Importing" << imported << "books take"
             << qPrintable(Utils::Time::prettyMilliSeconds(time.elapsed()));
}

void ImportDialog::doneImporting()
{
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

        QLabel *label = new QLabel(Utils::String::abbreviate(i.value(), 70));
        label->setStyleSheet("padding:5px;border:1px solid #cccccc;");

        int row = gridLayout->rowCount();
        gridLayout->addWidget(label, row, 0);
        gridLayout->addWidget(button, row, 1);

        connect(button, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
        m_signalMapper->setMapping(button, i.key());
    }

    IndexTracker::instance()->addTask(m_booksList.keys(), IndexTask::Add);
    IndexTracker::instance()->save();

    setModal(false);
    button(CancelButton)->hide();

    m_libraryManager->reloadManagers();

    next();

    m_progress.hide();
}

void ImportDialog::bookConverted(QString bookName)
{
    Q_UNUSED(bookName);

    m_progress.setValue(m_progress.value()+1);
}

void ImportDialog::addBooksToProgress(int count)
{
    m_progress.setMaximum(m_progress.maximum() + count);
}

void ImportDialog::selectFiles()
{
    QString lastPath = Utils::Settings::get("SavedPath/ImportDialog").toString();

    QStringList files = QFileDialog::getOpenFileNames(this,
                                                      tr("اختر الكتب التي تريد استيرادها:"),
                                                      lastPath,
                                                      "Supported formats(*.mlp *.bok);;"
                                                      "Moltaqa Library(*.mlp);;"
                                                      "Shamela books (*.bok)");
    foreach(QString file, files) {
        addFile(file);
    }

    if(files.size())
        Utils::Settings::set("SavedPath/ImportDialog",
                             QFileInfo(files.first()).absolutePath());
}

void ImportDialog::deleteFiles()
{
    QModelIndexList indexes = ui->fileListWidget->selectionModel()->selectedIndexes();
    for(int i=indexes.size()-1; i>=0; i--) {
        const QModelIndex index = indexes.at(i);
        ui->fileListWidget->takeItem(index.row());
    }
}

void ImportDialog::nextPage()
{
    ml_return_on_fail(validateCurrentPage());

    if (currentId() == Page_BookSelection)
        convertBooks();

     else if (currentId() == Page_ImportOption)
        importBooks();

     else if (currentId() == Page_ImportedBooks)
        next();
}
