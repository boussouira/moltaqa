#include "shamelaimportdialog.h"
#include "ui_shamelaimportdialog.h"
#include "shamelainfo.h"
#include "shamelamanager.h"
#include "utils.h"
#include "shamelaimportthread.h"
#include "mainwindow.h"
#include "indexdb.h"

#ifdef USE_MDBTOOLS
#include "mdbconverter.h"
#endif

#include <qdir.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qfuture.h>
#include <qtconcurrentrun.h>
#include <qstandarditemmodel.h>
#include <qsortfilterproxymodel.h>
#include <qmessagebox.h>
#include <qevent.h>

static ShamelaImportDialog* m_importDialog=0;

ShamelaImportDialog::ShamelaImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShamelaImportDialog)
{
    ui->setupUi(this);

    m_importDialog = this;

    m_shamela = new ShamelaInfo();
    m_manager = new ShamelaManager(m_shamela);
    m_importedBooksCount = 0;

    ui->groupImportOptions->setEnabled(false);
    ui->stackedWidget->setCurrentIndex(0);
    ui->pushDone->hide();

    connect(ui->pushNext, SIGNAL(clicked()), SLOT(nextStep()));
    connect(ui->pushCancel, SIGNAL(clicked()), SLOT(cancel()));
    connect(ui->pushDone, SIGNAL(clicked()), SLOT(accept()));
    connect(ui->buttonSelectShamela, SIGNAL(clicked()), SLOT(selectShamela()));
    connect(this, SIGNAL(setProgress(int)), ui->progressBar, SLOT(setValue(int)));
}

ShamelaImportDialog::~ShamelaImportDialog()
{
    delete m_shamela;
    delete m_manager;
    delete ui;
}

void ShamelaImportDialog::closeEvent(QCloseEvent *event)
{
    if(cancel()) {
        for(int i=0; i<m_importThreads.count(); i++) {
            if(m_importThreads.at(i)->isRunning())
                m_importThreads.at(i)->wait();
        }
        event->accept();
    } else {
        event->ignore();
    }
}

ShamelaImportDialog *ShamelaImportDialog::importDialog()
{
    return m_importDialog;
}

ShamelaManager *ShamelaImportDialog::shamelaManager()
{
    return m_manager;
}

ShamelaInfo *ShamelaImportDialog::shamelaInfo()
{
    return m_shamela;
}

LibraryInfo *ShamelaImportDialog::libraryInfo()
{
    return m_library;
}

bool ShamelaImportDialog::addAuthorsForEachBook()
{
    return ui->radioImportAuthorsWhenNedded->isChecked();
}

void ShamelaImportDialog::setLibraryInfo(LibraryInfo *info)
{
    m_library = info;
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

void ShamelaImportDialog::selectShamela()
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
                                 tr("لم تقم باختيار مجلد المكتبة الشاملة بشكل صحيح"));
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
                                 tr("لم تقم باختيار مجلد المكتبة الشاملة"));
        }
    } else if(index == 1) {
        showImportInfo();
        goPage();
    } else if(index == 2) { // Start importing
        goPage();
        setupImporting();
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
    if(ui->radioImportSelectionOnly->isChecked()) {
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
    } else {
        qDebug("* Import all books");
    }
}
void ShamelaImportDialog::setupImporting()
{
    LibraryCreator creator;
    creator.openDB();
    creator.createTables();

    creator.start();
    if(ui->radioImportAllAuthors->isChecked()){
        addDebugInfo(tr("جاري اضافة المؤلفيين..."));
        creator.importAuthors();
    }

    if(ui->radioUseShamelaCat->isChecked()) {
        addDebugInfo(tr("جاري استيراد الاقسام..."));
        creator.importCats();
    }

    creator.done();
}

void ShamelaImportDialog::showImportInfo()
{
    createFilter();

    int booksCount = m_manager->getBooksCount();
    int authorsCount = m_manager->getAuthorsCount();
    int catCount = m_manager->getCatCount();

    QString info = tr("<ul>");

    if(ui->checkImportQuran->isChecked())
        info += QString("<li>%1</li>").arg(tr("القرآن الكريم"));

    info += QString("<li>%1</li>").arg(Utils::arPlural(booksCount, Plural::BOOK));

    if(ui->radioImportAllAuthors->isChecked())
        info += QString("<li>%1</li>").arg(Utils::arPlural(authorsCount, Plural::AUTHOR));

    if(ui->radioUseShamelaCat->isChecked())
        info += tr("<li>سيتم اضافة %1 الى أقسام المكتبة الحالية</li>").arg(Utils::arPlural(catCount, Plural::CATEGORIE));

    ui->labelImportInfo->setText(info);
}


void ShamelaImportDialog::startImporting()
{
    m_importTime.start();

    ui->progressBar->setMaximum(m_manager->getBooksCount());
    ui->progressBar->setValue(0);
    ui->progressSteps->setValue(2);

#ifdef USE_MDBTOOLS
    m_importThreadCount = 1; // Don't use multi-threading with mdbtools
#else
    m_importThreadCount = QThread::idealThreadCount();
#endif

    m_manager->selectBooks();

    bool addQuarn = ui->checkImportQuran->isChecked();
    for(int i=0; i<m_importThreadCount; i++) {
        ShamelaImportThread *thread = new ShamelaImportThread(this);
        connect(thread, SIGNAL(bookImported(QString)), SLOT(bookImported(QString)));
        connect(thread, SIGNAL(doneImporting()), SLOT(doneImporting()));

        m_importThreads.append(thread);

        thread->setImportQuran(addQuarn);
        thread->start();

        addQuarn = false;
    }
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

void ShamelaImportDialog::bookImported(const QString &text)
{
    if(m_importedBooksCount == 0)
        ui->listDebug->addItem(tr("جاري استيراد الكتب..."));

    ui->progressBar->setValue(ui->progressBar->value()+1);
    ui->listDebug->addItem(" + "+text);
    ui->listDebug->scrollToBottom();

    m_importedBooksCount++;
}

void ShamelaImportDialog::doneImporting()
{
    if(--m_importThreadCount<=0) {
        ui->pushNext->hide();
        ui->pushCancel->hide();
        ui->pushDone->show();

        ui->progressBar->setValue(ui->progressBar->maximum());
        ui->progressSteps->setValue(ui->progressSteps->maximum());

        importShorooh();

        addDebugInfo(tr("تم استيراد %1 بنجاح خلال %2")
                     .arg(Utils::arPlural(m_importedBooksCount, Plural::BOOK))
                     .arg(Utils::secondsToString(m_importTime.elapsed())));

        if(m_importedBooksCount > 0) {
            MainWindow::mainWindow()->indexDB()->loadBooksListModel();
        }

        qDeleteAll(m_importThreads);
        m_importThreads.clear();

#ifdef USE_MDBTOOLS
        MdbConverter::removeAllConvertedDB();
#endif
    }
}

bool ShamelaImportDialog::cancel()
{
    if(m_importThreads.count()){
        int rep = QMessageBox::question(this,
                                        tr("ايقاف الاستيراد"),
                                        tr("هل تريد ايقاف استيراد الكتب؟"),
                                        QMessageBox::Yes|QMessageBox::No);
        if(rep == QMessageBox::Yes) {
            addDebugInfo(tr("جاري ايقاف الاستيراد..."));
            ui->pushCancel->setEnabled(false);
            for(int i=0; i<m_importThreads.count(); i++) {
                m_importThreads.at(i)->stop();
            }
        } else {
            return false;
        }
    } else {
        reject();
    }

    return true;
}

void ShamelaImportDialog::importShorooh()
{
    QList<uint> ad;
    addDebugInfo(tr("استيراد الشروح..."));
    LibraryCreator creator;
    creator.openDB();
    creator.start();

    foreach (ShamelaImportThread *thread, m_importThreads) {
        foreach (ShamelaShareehInfo *info, thread->getShorooh()) {
            // TODO: use a better way to check for added shorooh pages
            if(!ad.contains((info->shareeh_id << 15) | info->shareeh_page)) {
                creator.addShareh(info->mateen_id, info->mateen_page, info->shareeh_id, info->shareeh_page);
                ad.append((info->shareeh_id << 15) | info->shareeh_page);
            }
        }

        qDeleteAll(thread->getShorooh());
    }

    creator.done();
}
