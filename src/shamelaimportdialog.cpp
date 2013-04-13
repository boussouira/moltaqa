#include "shamelaimportdialog.h"
#include "ui_shamelaimportdialog.h"
#include "bookeditor.h"
#include "booklistmanager.h"
#include "checkablemessagebox.h"
#include "importdelegates.h"
#include "librarybook.h"
#include "librarybookmanager.h"
#include "libraryinfo.h"
#include "librarymanager.h"
#include "mainwindow.h"
#include "modelenums.h"
#include "modelviewfilter.h"
#include "shamelaimportinfo.h"
#include "shamelaimportthread.h"
#include "shamelainfo.h"
#include "shamelamanager.h"
#include "shamelamapper.h"
#include "sortfilterproxymodel.h"
#include "statisticsmanager.h"
#include "stringutils.h"
#include "taffesirlistmanager.h"
#include "timeutils.h"
#include "utils.h"

#ifdef USE_MDBTOOLS
#include "mdbconverter.h"
#endif

#include <qdir.h>
#include <qevent.h>
#include <qfiledialog.h>
#include <qfuture.h>
#include <qmessagebox.h>
#include <qprogressdialog.h>
#include <qscrollbar.h>
#include <qstandarditemmodel.h>
#include <qtconcurrentrun.h>

static ShamelaImportDialog* m_instance=0;

ShamelaImportDialog::ShamelaImportDialog(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::ShamelaImportDialog)
{
    ui->setupUi(this);

    ml_set_instance(m_instance, this);

    setOption(QWizard::DisabledBackButtonOnLastPage);

    m_shamela = new ShamelaInfo();
    m_manager = new ShamelaManager(m_shamela);
    m_libraryManager = LibraryManager::instance();
    m_bookListManager = m_libraryManager->bookListManager();
    m_bookManager = m_libraryManager->bookManager();
    m_library = MW->libraryInfo();

    m_bookFilter = new ModelViewFilter(this);

    m_importedBooksCount = 0;
    m_proccessItemChange = true;

    ui->radioUseShamelaCat->setChecked(!m_bookListManager->categoriesCount());

    ui->spinImportThreads->setMaximum(QThread::idealThreadCount() * 5);
    ui->spinImportThreads->setValue(Utils::Settings::get("ShamelaImportDialog/threadCount",
                                                         QThread::idealThreadCount()).toInt());
    ui->fileChooser->setLabelText(tr("مجلد المكتبة الشاملة:"));
    ui->fileChooser->setSettingName("ShamelaImportDialog");

    ui->pageImportProgress->setComplete(false);
    button(CancelButton)->disconnect();

#ifndef USE_MDBTOOLS
    ui->labelShamelaArchive->hide();
#endif

    connect(ui->pushSelectAll, SIGNAL(clicked()), SLOT(selectAllBooks()));
    connect(ui->pushSelectNew, SIGNAL(clicked()), SLOT(selectNewBooks()));
    connect(ui->pushUnSelectAll, SIGNAL(clicked()), SLOT(unSelectAllBooks()));
    connect(button(CancelButton), SIGNAL(clicked()), SLOT(cancel()));
}

ShamelaImportDialog::~ShamelaImportDialog()
{
    delete m_shamela;
    delete m_manager;
    delete ui;

    m_instance = 0;
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

bool ShamelaImportDialog::validateCurrentPage()
{
    if(currentId() == Page_ImportOption) {
        QString shamelaPath = ui->fileChooser->getPath();

        if(shamelaPath.size() && m_shamela->isShamelaPath(shamelaPath)) {
            m_manager->close();
            m_shamela->setShamelaPath(shamelaPath);
        } else {
            QMessageBox::warning(this,
                                 tr("الاستيراد من الشاملة"),
                                 tr("لم تقم باختيار مجلد المكتبة الشاملة بشكل صحيح"));
            return false;
        }

    } else if(currentId() == Page_BookSelection) {
        if(!createFilter()) {
            QMessageBox::warning(this,
                                 tr("الاستيراد من الشاملة"),
                                 tr("لم تقم باختيار اي كتاب!"));
            return false;
        }
    } else if(currentId() == Page_CategoriesLink) {
        if(ui->radioUseThisLibCat->isChecked() && !categorieLinked()) {
            int rep = CheckableMessageBox::question(this,
                                                    tr("الاستيراد من الشاملة"),
                                                    tr("لم تقم باختيار بعض الأقسام" "\n" "هل تريد المتابعة؟"),
                                                    "CheckableMessageBox/ShamelaImportCatLink",
                                                    QDialogButtonBox::Yes);
            if(rep == QDialogButtonBox::No)
                return false;
        }
    }

    return true;
}

int ShamelaImportDialog::nextId() const
{
    if(currentId() == Page_BookSelection && ui->radioUseShamelaCat->isChecked())
        return Page_ImportProgress;

    return QWizard::nextId();
}

void ShamelaImportDialog::closeEvent(QCloseEvent *event)
{
    if(cancel()) {
        for(int i=0; i<m_importThreads.count(); i++) {
            if(m_importThreads.at(i)->isRunning())
                m_importThreads.at(i)->wait();
        }

        Utils::Widget::save(this, "ShamelaImportDialog");

        Utils::Settings::set("ShamelaImportDialog/threadCount",
                             ui->spinImportThreads->value());

        event->accept();
    } else {
        event->ignore();
    }
}

void ShamelaImportDialog::initializePage(int id)
{
    if(id == Page_BookSelection) {
        showBooks();

        LibraryBook::Ptr quranBook = LibraryManager::instance()->bookManager()->getQuranBook();
        ui->checkImportQuran->setChecked(!quranBook || quranBook->id == -1);
    } else if(id == Page_CategoriesLink) {
        setupCategories();
    } else if(id == Page_ImportProgress) {
        setupImporting();
        startImporting();
    }
}

void ShamelaImportDialog::showBooks()
{
    m_booksModel = m_manager->getBooksListModel();

    m_bookFilter->setSourceModel(m_booksModel);
    m_bookFilter->setTreeView(ui->treeView);
    m_bookFilter->setLineEdit(ui->lineBookSearch);
    m_bookFilter->setup();

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

bool ShamelaImportDialog::categorieLinked()
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->tableView->model());
    ml_return_val_on_fail(model, false);

    QStandardItem *rootItem = model->invisibleRootItem();
    bool linked = true;
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
            addDebugInfo(tr("جاري استيراد الأقسام..."));
            creator.importCats();
        }

        creator.done();
    }
}

void ShamelaImportDialog::startImporting()
{
    m_importTime.start();

    bool addQuarn = ui->checkImportQuran->isChecked();
    int booksCount = m_manager->getBooksCount();

    if(addQuarn)
        booksCount++;

    ui->progressBar->setMaximum(booksCount);
    ui->progressBar->setValue(0);

    m_importThreadCount = qMin(ui->spinImportThreads->value(), booksCount);

    StatisticsManager::instance()->enqueue("ShamelaImport",
                                           QString("Import %1 books using %2 threads")
                                           .arg(booksCount)
                                           .arg(m_importThreadCount));

    m_manager->selectBooks();

    for(int i=0; i<m_importThreadCount; i++) {
        ShamelaImportThread *thread = new ShamelaImportThread(this);
        connect(thread, SIGNAL(bookImported(QString)), SLOT(bookImported(QString)));
        connect(thread, SIGNAL(BookImportError(QString)), SLOT(BookImportError(QString)));
        connect(thread, SIGNAL(doneImporting()), SLOT(doneImporting()));

        m_importThreads.append(thread);

        thread->setImportQuran(addQuarn);
        thread->setThreadId(i+1);
        thread->start();

        addQuarn = false;
    }
}

void ShamelaImportDialog::addDebugInfo(const QString &text, QIcon icon)
{
    bool scrollToBottom = (ui->listDebug->verticalScrollBar()->maximum()
                           == ui->listDebug->verticalScrollBar()->value());

    if(icon.isNull())
        ui->listDebug->addItem(text);
    else
        ui->listDebug->addItem(new QListWidgetItem(icon, text));

    if(scrollToBottom)
        ui->listDebug->scrollToBottom();
}

void ShamelaImportDialog::bookImported(const QString &text)
{
    if(m_importedBooksCount == 0)
        addDebugInfo(tr("جاري استيراد الكتب..."));

    ui->progressBar->setValue(ui->progressBar->value()+1);
    m_importedBooksCount++;

    addDebugInfo(text, QIcon(":/images/add2.png"));
}

void ShamelaImportDialog::BookImportError(const QString &text)
{
    m_importError.append(text);

    addDebugInfo(tr("حدث خطأ أثناء استيراد '%1'").arg(text),
                 QIcon(":/images/alert.png"));
}

void ShamelaImportDialog::doneImporting()
{
    if(--m_importThreadCount<=0) {
        ui->progressBar->setMaximum(m_importedBooksCount);
        ui->progressBar->setValue(ui->progressBar->maximum());

        if(m_importedBooksCount > 0) {
            m_libraryManager->reloadManagers();
            importShorooh();
        } else {
            qDebug("ShamelaImportDialog::doneImporting no book imported");
        }

        if(m_importError.size()) {
            addDebugInfo(tr("لم يتم استيراد %1:")
                         .arg(Utils::String::Arabic::plural(m_importError.size(), Utils::String::Arabic::BOOK)));

            foreach (QString title, m_importError) {
                addDebugInfo(title, QIcon(":/images/delete2.png"));
            }

            qWarning() << "ShamelaImportDialog::doneImporting"
                       << tr("لم يتم استيراد الكتب التالية:") + "\n" + m_importError.join("\n");
        }

        addDebugInfo(tr("تم استيراد %1 بنجاح خلال %2")
                     .arg(Utils::String::Arabic::plural(m_importedBooksCount, Utils::String::Arabic::BOOK))
                     .arg(Utils::Time::secondsToString(m_importTime.elapsed())));

        QString str = QString("%1 books imported in %2")
                .arg(m_importedBooksCount)
                .arg(Utils::Time::prettyMilliSeconds(m_importTime.elapsed()));

        if(m_importError.size())
            str.replace("books", QString("books (%1 errors)").arg(m_importError.size()));

        StatisticsManager::instance()->dequeue("ShamelaImport", str);

        qDeleteAll(m_importThreads);
        m_importThreads.clear();

#ifdef USE_MDBTOOLS
        MdbConverter::removeAllConvertedDB();
#endif

        ui->pageImportProgress->setComplete(true);
        button(CancelButton)->hide();

        Utils::Settings::set("ShamelaImportDialog/threadCount",
                             ui->spinImportThreads->value());

        CheckableMessageBox::information(this,
                                         tr("الاستيراد من الشاملة"),
                                         tr("سيتم فهرسة الكتب التي تم استيرادها بعد اعادة تشغيل البرنامج"),
                                         "CheckableMessageBox/ShamelaImportDone");
    }
}

void ShamelaImportDialog::importShorooh()
{
    addDebugInfo(tr("استيراد الشروح..."));

    m_manager->importShorooh(this);
}

bool ShamelaImportDialog::cancel()
{
    if(m_importThreads.count()){
        int rep = QMessageBox::question(this,
                                        tr("ايقاف الاستيراد"),
                                        tr("هل تريد ايقاف استيراد الكتب؟"),
                                        QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
        if(rep == QMessageBox::Yes) {
            addDebugInfo(tr("جاري ايقاف الاستيراد..."));
            button(CancelButton)->setEnabled(false);
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

void ShamelaImportDialog::selectNewBooks()
{
    QProgressDialog dialog(this);
    dialog.setWindowTitle(tr("الكتب الجديدة"));
    dialog.setLabelText(tr("جاري البحث عن الكتب الجديدة..."));
    dialog.setMaximum(m_manager->getBooksCount());
    dialog.setCancelButton(0);

    int selectedCount = 0;
    QStandardItem *item = m_booksModel->invisibleRootItem();
    for(int i=0; i<item->rowCount(); i++)
        selectedCount += selectNewBook(item->child(i, 0), &dialog);

    QMessageBox::information(this,
                             tr("الكتب الجديدة"),
                             tr("عدد الكتب الجديدة %1""<br/>"
                                "عدد الكتب المكررة %2")
                             .arg(Utils::String::Arabic::plural(selectedCount, Utils::String::Arabic::BOOK, true))
                             .arg(Utils::String::Arabic::plural(m_manager->getBooksCount() - selectedCount,
                                                                Utils::String::Arabic::BOOK, true)));
}

int ShamelaImportDialog::selectNewBook(QStandardItem *item, QProgressDialog *progress)
{
    int selectedCount = 0;

    if(item->data(ItemRole::typeRole).toInt() == ItemType::BookItem) {
        if(m_bookManager->findBook(item->text().trimmed())) {
            item->setCheckState(Qt::Unchecked);
        } else {
            item->setCheckState(Qt::Checked);
            selectedCount++;
        }

        progress->setValue(progress->value()+1);
    }

    for(int i=0; i<item->rowCount(); i++)
        selectedCount += selectNewBook(item->child(i, 0), progress);

    qApp->processEvents();

    return selectedCount;
}
