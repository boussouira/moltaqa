#include "shamelaimportdialog.h"
#include "ui_shamelaimportdialog.h"
#include "shamelainfo.h"
#include "shamelamanager.h"
#include "utils.h"
#include "stringutils.h"
#include "timeutils.h"
#include "shamelaimportthread.h"
#include "mainwindow.h"
#include "librarymanager.h"
#include "librarybook.h"
#include "sortfilterproxymodel.h"
#include "importdelegates.h"
#include "taffesirlistmanager.h"
#include "booklistmanager.h"
#include "librarybookmanager.h"
#include "modelenums.h"
#include "bookeditor.h"

#ifdef USE_MDBTOOLS
#include "mdbconverter.h"
#endif

#include <qdir.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qfuture.h>
#include <qtconcurrentrun.h>
#include <qstandarditemmodel.h>
#include <qmessagebox.h>
#include <qevent.h>
#include <qsettings.h>

static ShamelaImportDialog* m_instance=0;

ShamelaImportDialog::ShamelaImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShamelaImportDialog)
{
    ui->setupUi(this);

    m_instance = this;

    m_shamela = new ShamelaInfo();
    m_manager = new ShamelaManager(m_shamela);
    m_libraryManager = LibraryManager::instance();
    m_bookListManager = m_libraryManager->bookListManager();
    m_library = MW->libraryInfo();

    m_importedBooksCount = 0;
    m_proccessItemChange = true;

    ui->radioUseShamelaCat->setChecked(!m_bookListManager->categoriesCount());
    ui->groupImportOptions->setEnabled(false);
    ui->stackedWidget->setCurrentIndex(0);
    ui->pushDone->hide();

    QSettings settings;
    ui->spinImportThreads->setMaximum(QThread::idealThreadCount() * 5);
    ui->spinImportThreads->setValue(settings.value("ShamelaImportDialog/threadCount",
                                                   QThread::idealThreadCount() * 2).toInt());

    Utils::Widget::restore(this, "ShamelaImportDialog");

    connect(ui->pushNext, SIGNAL(clicked()), SLOT(nextStep()));
    connect(ui->pushCancel, SIGNAL(clicked()), SLOT(cancel()));
    connect(ui->pushDone, SIGNAL(clicked()), SLOT(accept()));
    connect(ui->buttonSelectShamela, SIGNAL(clicked()), SLOT(selectShamela()));
    connect(this, SIGNAL(setProgress(int)), ui->progressBar, SLOT(setValue(int)));
    connect(ui->pushSelectAll, SIGNAL(clicked()), SLOT(selectAllBooks()));
    connect(ui->pushUnSelectAll, SIGNAL(clicked()), SLOT(unSelectAllBooks()));
}

ShamelaImportDialog::~ShamelaImportDialog()
{
    delete m_shamela;
    delete m_manager;
    delete ui;

    m_instance = 0;
}

void ShamelaImportDialog::closeEvent(QCloseEvent *event)
{
    if(cancel()) {
        for(int i=0; i<m_importThreads.count(); i++) {
            if(m_importThreads.at(i)->isRunning())
                m_importThreads.at(i)->wait();
        }

        Utils::Widget::save(this, "ShamelaImportDialog");

        QSettings settings;
        settings.setValue("ShamelaImportDialog/threadCount",
                          ui->spinImportThreads->value());

        event->accept();
    } else {
        event->ignore();
    }
}

ShamelaImportDialog *ShamelaImportDialog::instance()
{
    Q_CHECK_PTR(m_instance);

    return m_instance;
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

    if(dirPath.size()) {
        QDir dir(dirPath);
        return dir.absolutePath();
    } else {
        return QString();
    }
}

void ShamelaImportDialog::selectShamela()
{
    QSettings settings;
    QString lastPath = settings.value("SavedPath/ShamelaImportDialog",
                                      ui->lineShamelaDir->text()).toString();

    QString path = getFolderPath(lastPath);

    if(path.size()){
        if(m_shamela->isShamelaPath(path)) {
            ui->lineShamelaDir->setText(QDir::toNativeSeparators(path));
            ui->groupImportOptions->setEnabled(true);

            m_manager->close();
            m_shamela->setShamelaPath(path);

            settings.setValue("SavedPath/ShamelaImportDialog", path);
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
        if(m_shamela->shamelaPath().size()) {
            showBooks();
            goPage();
        } else {
            QMessageBox::warning(this,
                                 tr("الاستيراد من الشاملة"),
                                 tr("لم تقم باختيار مجلد المكتبة الشاملة"));
        }
    } else if(index == 1) {
        if(!createFilter()) {
            QMessageBox::warning(this,
                                 tr("الاستيراد من الشاملة"),
                                 tr("لم تقم باختيار اي كتاب!"));
            return;
        }

        if(ui->radioUseThisLibCat->isChecked()) {
            setupCategories();
            goPage();
        } else {
            goPage();
            nextStep();
        }
    } else if(index == 2) { // Start importing
        if(ui->radioUseThisLibCat->isChecked()) {
            if(!categorieLinked()) {
                if(QMessageBox::question(this,
                                         tr("الاستيراد من الشاملة"),
                                         tr("لم تقم باختيار بعض الاقسام" "\n"
                                            "هل تريد المتابعة؟"),
                                         QMessageBox::Yes|QMessageBox::No, QMessageBox::No) == QMessageBox::No)
                    return;
            }
        }

        setupImporting();
        startImporting();
        ui->pushNext->setEnabled(false);
        goPage();
    }
}

void ShamelaImportDialog::goPage(int index)
{
    int currentIndex = (index == -1) ? ui->stackedWidget->currentIndex()+1 : index;

    ui->stackedWidget->setCurrentIndex(currentIndex);
}

void ShamelaImportDialog::showBooks()
{
    m_booksModel = m_manager->getBooksListModel();
    SortFilterProxyModel *filterModel = new SortFilterProxyModel(this);

    filterModel->setSourceModel(m_booksModel);
    ui->treeView->setModel(filterModel);

    m_booksModel->setHeaderData(0, Qt::Horizontal, tr("لائحة الكتب"), Qt::DisplayRole);

    LibraryBookPtr quranBook = LibraryManager::instance()->bookManager()->getQuranBook();
    ui->checkImportQuran->setChecked(!quranBook || quranBook->id == -1);

    connect(ui->lineBookSearch, SIGNAL(textChanged(QString)), filterModel, SLOT(setFilterRegExp(QString)));
    connect(ui->lineBookSearch, SIGNAL(textChanged(QString)), ui->treeView, SLOT(expandAll()));
    connect(m_booksModel, SIGNAL(itemChanged(QStandardItem*)), SLOT(itemChanged(QStandardItem*)));

    if(m_bookListManager->booksCount() < m_manager->getBooksCount())
        selectAllBooks();
}

bool ShamelaImportDialog::createFilter()
{
        QList<int> selectedIDs;

        for(int i=0; i<m_booksModel->rowCount(); i++) {
            QStandardItem *item = m_booksModel->item(i);
            if(item->data(ItemRole::typeRole).toInt() == ItemType::CategorieItem) {
                for(int j=0; j < item->rowCount(); j++) {
                    QStandardItem *child = item->child(j);
                    if(child->checkState() == Qt::Checked){
                        selectedIDs.append(child->data(ItemRole::idRole).toInt());
                    }
                }
            } else if(item->data(ItemRole::typeRole).toInt() == ItemType::BookItem) {
                if(item->checkState() == Qt::Checked){
                    selectedIDs.append(item->data(ItemRole::idRole).toInt());
                }
            }
        }

        m_manager->setFilterBooks(true);
        m_manager->setAcceptedBooks(selectedIDs);

        return selectedIDs.isEmpty() ? ui->checkImportQuran->isChecked() : true;
}

void ShamelaImportDialog::setupImporting()
{
    if(ui->radioUseThisLibCat->isChecked()) {
        QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->tableView->model());
        QStandardItem *rootItem = model->invisibleRootItem();
        for(int i=0; i < rootItem->rowCount(); i++) {
            QStandardItem *shamelaItem = rootItem->child(i, 0);
            QStandardItem* libraryItem = rootItem->child(i, 1);

            if(libraryItem && shamelaItem)
                m_manager->mapper()->addCatMap(shamelaItem->data(ItemRole::idRole).toInt(),
                                               libraryItem->data(ItemRole::idRole).toInt());
        }
    } else {
        LibraryCreator creator;
        creator.openDB();

        creator.start();

        if(ui->radioUseShamelaCat->isChecked()) {
            addDebugInfo(tr("جاري استيراد الاقسام..."));
            creator.importCats();
        }

        creator.done();
    }
}

void ShamelaImportDialog::setupCategories()
{
    QStandardItemModel *model = new QStandardItemModel;
    model->setHorizontalHeaderLabels(QStringList()
                                     << tr("المكتبة الشاملة")
                                     << tr("المكتبة الحالية"));

    ui->tableView->setItemDelegateForColumn(1, new CategorieDelegate(this));
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->verticalHeader()->setVisible(false);

    // Get shamela categories
    m_manager->selectCats();

    ShamelaCategorieInfo *cat = m_manager->nextCat();
    while(cat) {
        QStandardItem* libraryItem = 0;
        QStandardItem *shamelaItem = new QStandardItem;
        shamelaItem->setText(cat->name);
        shamelaItem->setData(cat->id, ItemRole::idRole);
        shamelaItem->setEditable(false);

        // Try to find this cat in our library
        CategorieInfo *libCat = m_bookListManager->findCategorie(cat->name);
        if(libCat) {
            libraryItem = new QStandardItem;
            libraryItem->setText(libCat->title);
            libraryItem->setData(libCat->catID, ItemRole::idRole);
        }

        model->setItem(model->rowCount(), 0, shamelaItem);

        if(libraryItem)
            model->setItem(model->rowCount()-1, 1, libraryItem);

        delete cat;
        cat = m_manager->nextCat();
    }

    ui->tableView->setModel(model);
    ui->tableView->resizeColumnsToContents();
}

void ShamelaImportDialog::startImporting()
{
    m_importTime.start();

    ui->progressBar->setMaximum(m_manager->getBooksCount());
    ui->progressBar->setValue(0);

    m_importThreadCount = ui->spinImportThreads->value();

    qDebug("ShamelaImportDialog: Start importing %d books using %d threads",
           m_manager->getBooksCount(),
           m_importThreadCount);

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
    ui->listDebug->addItem(new QListWidgetItem(QIcon(":/images/add2.png"), text));
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

        if(m_importedBooksCount > 0) {
            m_libraryManager->reloadManagers();
            importShorooh();
        } else {
            qDebug("ShamelaImportDialog::doneImporting no book imported");
        }

        addDebugInfo(tr("تم استيراد %1 بنجاح خلال %2")
                     .arg(Utils::String::Arabic::plural(m_importedBooksCount, Utils::String::Arabic::BOOK))
                     .arg(Utils::Time::secondsToString(m_importTime.elapsed())));

        qDeleteAll(m_importThreads);
        m_importThreads.clear();

#ifdef USE_MDBTOOLS
        MdbConverter::removeAllConvertedDB();
#endif

        QMessageBox::information(this,
                                 tr("الاستيراد من الشاملة"),
                                 tr("سيتم فهرسة الكتب التي تم استيرادها بعد اعادة تشغيل البرنامج"));
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
    addDebugInfo(tr("استيراد الشروح..."));

    m_manager->importShorooh();
}

void ShamelaImportDialog::itemChanged(QStandardItem *item)
{
    if(item && m_proccessItemChange) {
        m_proccessItemChange = false;

        if(item->data(ItemRole::typeRole).toInt() == ItemType::CategorieItem) {
            for(int i=0; i<item->rowCount(); i++) {
                item->child(i)->setCheckState(item->checkState());
            }
        } else if(item->data(ItemRole::typeRole).toInt() == ItemType::BookItem) {
            QStandardItem *parentItem = item->parent();
            int checkItems = 0;

            for(int i=0; i<parentItem->rowCount(); i++) {
                if(parentItem->child(i)->checkState()==Qt::Checked)
                    checkItems++;
            }

            if(checkItems == 0)
                parentItem->setCheckState(Qt::Unchecked);
            else if(checkItems < parentItem->rowCount())
                parentItem->setCheckState(Qt::PartiallyChecked);
            else
                parentItem->setCheckState(Qt::Checked);

        }

        m_proccessItemChange = true;
    }
}

void ShamelaImportDialog::selectAllBooks()
{
    int rowCount = m_booksModel->rowCount();
    QModelIndex topLeft = m_booksModel->index(0, 0);
    QModelIndex bottomRight = m_booksModel->index(rowCount-1, 0);
    QItemSelection selection(topLeft, bottomRight);

    foreach (QModelIndex index, selection.indexes()) {
        m_booksModel->setData(index, Qt::Checked, Qt::CheckStateRole);
    }
}

void ShamelaImportDialog::unSelectAllBooks()
{
    int rowCount = m_booksModel->rowCount();
    QModelIndex topLeft = m_booksModel->index(0, 0);
    QModelIndex bottomRight = m_booksModel->index(rowCount-1, 0);
    QItemSelection selection(topLeft, bottomRight);

    foreach (QModelIndex index, selection.indexes()) {
        m_booksModel->setData(index, Qt::Unchecked, Qt::CheckStateRole);
    }
}

bool ShamelaImportDialog::categorieLinked()
{
    bool linked = true;
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->tableView->model());
    QStandardItem *rootItem = model->invisibleRootItem();
    for(int i=0; i < rootItem->rowCount(); i++) {
        QStandardItem *shamelaItem = rootItem->child(i, 0);
        QStandardItem *libraryCat = rootItem->child(i, 1);
        if(!libraryCat) {
            linked = false;
            if(shamelaItem)
                shamelaItem->setBackground(Qt::lightGray);
        } else {
            if(shamelaItem)
                shamelaItem->setBackground(Qt::white);
        }
    }

    return linked;
}
