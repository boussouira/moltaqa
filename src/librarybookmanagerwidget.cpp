#include "librarybookmanagerwidget.h"
#include "ui_librarybookmanagerwidget.h"
#include "librarymanager.h"
#include "librarybook.h"
#include "selectauthordialog.h"
#include "librarybookmanager.h"
#include "modelenums.h"
#include "utils.h"
#include "editwebview.h"
#include "modelutils.h"
#include "modelviewfilter.h"
#include "newbookwriter.h"
#include "mainwindow.h"
#include "booklistmanager.h"
#include "bookreaderview.h"
#include "authorsmanager.h"

#include <qdebug.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qmessagebox.h>
#include <QInputDialog>
#include <QHBoxLayout>
#include <indextracker.h>

LibraryBookManagerWidget::LibraryBookManagerWidget(QWidget *parent) :
    ControlCenterWidget(parent),
    ui(new Ui::LibraryBookManagerWidget),
    m_currentBook(0),
    m_model(0),
    m_webEdit(0)
{
    ui->setupUi(this);

    m_manager = LibraryManager::instance()->bookManager();
    m_filter = new ModelViewFilter(this);

    m_selectCurrentBook = true;

    enableEditWidgets(false);
    setupActions();
    setupBookReader();
}

LibraryBookManagerWidget::~LibraryBookManagerWidget()
{
    ml_delete_check(m_model);

    delete ui;
}

QString LibraryBookManagerWidget::title()
{
    return tr("الكتب");
}

void LibraryBookManagerWidget::aboutToShow()
{
    ml_return_on_fail(!m_webEdit);

    m_webEdit = new EditWebView(this);

    QVBoxLayout *layout = new QVBoxLayout(ui->tabBookInfo);
    layout->addWidget(m_webEdit);

    if(Utils::Settings::contains("BookManagerWidget/splitter"))
        ui->splitter->restoreState(Utils::Settings::get("BookManagerWidget/splitter").toByteArray());

    if(m_selectCurrentBook) {
        if(MW->bookReaderView()->currentBookID()) {
            QModelIndex index = Utils::Model::findModelIndex(ui->treeView->model(),
                                                             MW->bookReaderView()->currentBookID());
            if(index.isValid()) {
                Utils::Model::selectIndex(ui->treeView, index);
                on_treeView_doubleClicked(index);
            }
        }

        m_selectCurrentBook = false;
    }
}

void LibraryBookManagerWidget::aboutToHide()
{
    Utils::Settings::set("BookManagerWidget/splitter", ui->splitter->saveState());
}

void LibraryBookManagerWidget::setupActions()
{
     foreach(QLineEdit *edit, findChildren<QLineEdit *>()) {
         connect(edit, SIGNAL(textChanged(QString)), SLOT(infoChanged()));
     }

     foreach(QTextEdit *edit, findChildren<QTextEdit *>()) {
         connect(edit, SIGNAL(textChanged()), SLOT(infoChanged()));
     }

     foreach (QCheckBox *check, findChildren<QCheckBox *>()) {
         connect(check, SIGNAL(stateChanged(int)), SLOT(infoChanged()));
     }

     foreach (QComboBox *combo, findChildren<QComboBox *>()) {
         connect(combo, SIGNAL(currentIndexChanged(int)), SLOT(infoChanged()));
     }

     connect(ui->tabWidget, SIGNAL(currentChanged(int)), SLOT(checkEditWebChange()));
     connect(ui->toolAdd, SIGNAL(clicked()), SLOT(createNewBook()));
     connect(ui->toolDelete, SIGNAL(clicked()), SLOT(removeBook()));
     connect(ui->toolReIndex, SIGNAL(clicked()), SLOT(reindexBook()));
}

void LibraryBookManagerWidget::setupBookReader()
{
    m_readerview = new BookReaderView(LibraryManager::instance(), this);

    m_readerWidget = new QWidget(this);
    QWidget *toolBarWidget = new QWidget(this);
    QHBoxLayout *toolBarLayout = new QHBoxLayout;
    QVBoxLayout *mainlayout = new QVBoxLayout;

    foreach(QToolBar *bar, m_readerview->toolBars()) {
        toolBarLayout->addWidget(bar);
    }

    toolBarLayout->addStretch(1);
    toolBarLayout->setContentsMargins(9, 0, 9, 0);
    toolBarLayout->setSpacing(0);
    toolBarWidget->setLayout(toolBarLayout);

    mainlayout->setMenuBar(toolBarWidget);
    mainlayout->addWidget(m_readerview);

    m_readerWidget->setLayout(mainlayout);
    ui->tabWidget->addTab(m_readerWidget, tr("تصفح الكتاب"));

    connect(m_readerview->bookWidgetManager(), SIGNAL(lastTabClosed()), SLOT(lastReaderTabClosed()));
}

void LibraryBookManagerWidget::enableEditWidgets(bool enable)
{
    ui->tabWidget->setEnabled(enable);
}

void LibraryBookManagerWidget::loadModel()
{
    ml_delete_check(m_model);

    m_model = Utils::Model::cloneModel(m_manager->getModel().data());

    m_filter->reset();
    m_filter->addFilterColumn(0, Qt::DisplayRole, tr("عنوان الكتاب"));
    m_filter->addFilterColumn(1, Qt::DisplayRole, tr("اسم المؤلف"));

    m_filter->setSourceModel(m_model);
    m_filter->setLineEdit(ui->lineFilter);
    m_filter->setTreeView(ui->treeView);
    m_filter->setup();
}

void LibraryBookManagerWidget::infoChanged()
{
    if(m_currentBook) {
        m_editedBookInfo[m_currentBook->id] = m_currentBook;

        setModified(true);
    }
}

void LibraryBookManagerWidget::checkEditWebChange()
{
    ml_return_on_fail(m_webEdit);

    if(m_webEdit->pageModified())
        infoChanged();
}

void LibraryBookManagerWidget::createNewBook()
{
    bool ok;
    QString title = QInputDialog::getText(this,
                                          tr("كتاب جديد"),
                                          tr("عنوان الكتاب الجديد:"),
                                          QLineEdit::Normal, QString(), &ok).trimmed();
    ml_return_on_fail(ok);

    if(title.isEmpty()) {
        QMessageBox::warning(this,
                             tr("كتاب جديد"),
                             tr("لم تقم باختيار عنوان الكتاب"));

        return;
    }

    try {

        LibraryBook::Ptr book(new LibraryBook());
        book->title = title;
        book->type = LibraryBook::NormalBook;

        selectAuthorDialog authorDialog(this);
        if(authorDialog.exec() == QDialog::Accepted) {
            book->authorID = authorDialog.selectedAuthorID();
            book->authorName = authorDialog.selectedAuthorName();
        }

        BookPage page;
        page.pageID = 1;
        page.page = 1;
        page.part = 1;

        NewBookWriter bookWrite;
        bookWrite.createNewBook();

        bookWrite.startReading();

        bookWrite.addPage(&page);
        bookWrite.addTitle(title, 1, 0);
        bookWrite.writeMetaFiles();

        bookWrite.addPageText(1, title);

        bookWrite.endReading();

        book->path = bookWrite.bookPath();
        book->fileName = QFileInfo(book->path).fileName();

        m_libraryManager->addBook(book, 0);
        m_libraryManager->bookListManager()->reloadModels();

        QList<QStandardItem*> rows;
        QStandardItem *item = new QStandardItem(book->title);
        item->setData(book->id, ItemRole::idRole);
        item->setIcon(QIcon(":/images/book.png"));
        rows << item;

        AuthorInfo::Ptr author = m_libraryManager->authorsManager()->getAuthorInfo(book->authorID);
        if(author) {
            QStandardItem *authorItem = new QStandardItem();
            authorItem->setText(author->name);
            authorItem->setData(author->id, ItemRole::authorIdRole);
            rows << authorItem;
        }

        m_model->appendRow(rows);

        QModelIndex index = m_model->indexFromItem(item);
        if(index.isValid()) {
            Utils::Model::selectIndex(ui->treeView, index);
            ui->treeView->scrollToBottom();
            on_treeView_doubleClicked(index);
        }

        QMessageBox::information(this,
                                 tr("كتاب جديد"),
                                 tr("تم انشاء الكتاب الجديد"));
    } catch(BookException &e) {
        e.showMessage(tr("كتاب جديد"), this);
        e.print();
    }
}

void LibraryBookManagerWidget::removeBook()
{
    QModelIndex index = Utils::Model::selectedIndex(ui->treeView);
    if(index.isValid()) {
        if(QMessageBox::question(this,
                                 tr("حذف كتاب"),
                                 tr("هل انت متأكد من انك تريد حذف '%1'؟")
                                 .arg(index.data().toString()),
                                 QMessageBox::Yes|QMessageBox::No,
                                 QMessageBox::No)==QMessageBox::Yes) {
            int bookId = index.data(ItemRole::idRole).toInt();
            QModelIndex sourceIndex = m_filter->filterModel()->mapToSource(index);
            m_model->removeRow(sourceIndex.row(), sourceIndex.parent());

            m_libraryManager->removeBook(bookId);
        }
    } else {
        QMessageBox::warning(this,
                             tr("حذف كتاب"),
                             tr("لم تقم باختيار اي كتاب"));
    }
}

void LibraryBookManagerWidget::reindexBook()
{
    QModelIndex index = Utils::Model::selectedIndex(ui->treeView);
    if(index.isValid()) {
        if(QMessageBox::question(this,
                                 tr("إعادة فهرسة كتاب"),
                                 tr("هل تريد إعادة فهرس كتاب '%1'؟")
                                 .arg(index.data().toString()),
                                 QMessageBox::Yes|QMessageBox::No,
                                 QMessageBox::No)==QMessageBox::Yes) {
            int bookId = index.data(ItemRole::idRole).toInt();
            if(bookId) {
                IndexTracker::instance()->addTask(bookId, IndexTask::Update, false);
                QMessageBox::information(this,
                                         tr("إعادة فهرسة كتاب"),
                                         tr("ستتم إعادة فهرسة هذا الكتاب عند إعادة تشغيل البرنامج"));
            } else {
                QMessageBox::warning(this,
                                     tr("حذف كتاب"),
                                     tr("لم تقم باختيار اي كتاب"));
            }
        }
    }
}

void LibraryBookManagerWidget::lastReaderTabClosed()
{
    m_readerWidget->setEnabled(false);
}

void LibraryBookManagerWidget::save()
{
    saveCurrentBookInfo();

    if(m_editedBookInfo.size()) {
        m_manager->transaction();

        foreach(LibraryBook::Ptr book, m_editedBookInfo.values()) {
            m_manager->updateBook(book, false);
        }

        m_manager->commit();
        m_manager->reloadModels();

        m_editedBookInfo.clear();
        m_currentBook.clear();

        setModified(false);
    }
}

void LibraryBookManagerWidget::on_treeView_doubleClicked(const QModelIndex &index)
{
    QModelIndex bookIndex = (index.column() != 0)
                             ? index.sibling(index.row(), 0)
                             : index;

    int bookID = bookIndex.data(ItemRole::idRole).toInt();
    LibraryBook::Ptr info = getBookInfo(bookID);
    if(info) {
        saveCurrentBookInfo();

        // this will cause infoChanged() to ignore changes that we will made next
        m_currentBook.clear();

        ui->lineDisplayName->setText(info->title);
        ui->lineAuthorName->setText(info->authorName);
        m_webEdit->setEditorText(info->info);
        ui->plainBookNames->setText(info->otherTitles.replace(';', '\n'));
        ui->lineEdition->setText(info->edition);
        ui->lineMohaqeq->setText(info->mohaqeq);
        ui->linePublisher->setText(info->publisher);
        ui->plainBookComment->setPlainText(info->comment);

        if(info->bookFlags & LibraryBook::PrintedPageNumber)
            ui->comboPageNumber->setCurrentIndex(1);
        else if(info->bookFlags & LibraryBook::MakhetotPageNumer)
            ui->comboPageNumber->setCurrentIndex(2);
        else
            ui->comboPageNumber->setCurrentIndex(0);

        if(info->bookFlags & LibraryBook::MoqabalMoteboa)
            ui->comboMoqabal->setCurrentIndex(1);
        else if(info->bookFlags & LibraryBook::MoqabalMakhetot)
            ui->comboMoqabal->setCurrentIndex(2);
        else if(info->bookFlags & LibraryBook::MoqabalPdf)
            ui->comboMoqabal->setCurrentIndex(3);
        else
            ui->comboMoqabal->setCurrentIndex(0);

        ui->checkLinkedWithshareeh->setChecked(info->bookFlags & LibraryBook::LinkedWithShareeh);
        ui->checkHaveFootnote->setChecked(info->bookFlags & LibraryBook::HaveFootNotes);
        ui->checkMashekol->setChecked(info->bookFlags & LibraryBook::Mashekool);

        enableEditWidgets(true);
        setupEdit(info);
        m_webEdit->setBook(info);

        m_currentBook = info;
    }
}

void LibraryBookManagerWidget::on_toolChangeAuthor_clicked()
{
    selectAuthorDialog dialog(this);

    if(dialog.exec() == QDialog::Accepted) {
        ui->lineAuthorName->setText(dialog.selectedAuthorName());
        if(m_currentBook) {
            m_currentBook->authorID = dialog.selectedAuthorID();
            m_currentBook->authorName = dialog.selectedAuthorName();
        }
    }
}

void LibraryBookManagerWidget::setupEdit(LibraryBook::Ptr info)
{
    bool enable = !info->isQuran();

    ui->lineAuthorName->setEnabled(enable);
    ui->toolChangeAuthor->setEnabled(enable);
    ui->plainBookNames->setEnabled(enable);
    ui->lineEdition->setEnabled(enable);
    ui->lineMohaqeq->setEnabled(enable);
    ui->linePublisher->setEnabled(enable);
    ui->widgetBookMeta->setEnabled(enable);
}

void LibraryBookManagerWidget::saveCurrentBookInfo()
{
    ui->tabWidget->setCurrentIndex(0);

    if(m_currentBook) {
        m_currentBook->title = ui->lineDisplayName->text().simplified();
        m_currentBook->authorName = ui->lineAuthorName->text().simplified();
        m_currentBook->info = m_webEdit->editorText();
        m_currentBook->otherTitles = ui->plainBookNames->toPlainText().replace('\n', ';');
        m_currentBook->edition = ui->lineEdition->text().simplified();
        m_currentBook->mohaqeq = ui->lineMohaqeq->text().simplified();
        m_currentBook->publisher = ui->linePublisher->text().simplified();
        m_currentBook->comment = ui->plainBookComment->toPlainText();

        m_currentBook->bookFlags = LibraryBook::NoBookFlags;

        int flags = 0;

        switch (ui->comboPageNumber->currentIndex()) {
        case 1:
            flags |= LibraryBook::PrintedPageNumber;
            break;
        case 2:
            flags |= LibraryBook::MakhetotPageNumer;
            break;
        default:
            break;
        }

        switch (ui->comboMoqabal->currentIndex()) {
        case 1:
            flags |= LibraryBook::MoqabalMoteboa;
            break;
        case 2:
            flags |= LibraryBook::MoqabalMakhetot;
            break;
        case 3:
            flags |= LibraryBook::MoqabalPdf;
            break;
        default:
            break;
        }

        if(ui->checkLinkedWithshareeh->isChecked())
            flags |= LibraryBook::LinkedWithShareeh;

        if(ui->checkHaveFootnote->isChecked())
                flags |= LibraryBook::HaveFootNotes;

        if(ui->checkMashekol->isChecked())
                flags |= LibraryBook::Mashekool;

        m_currentBook->bookFlags = static_cast<LibraryBook::BookFlags>(flags);
    }
}

LibraryBook::Ptr LibraryBookManagerWidget::getBookInfo(int bookID)
{
    LibraryBook::Ptr info = m_editedBookInfo.value(bookID);
    if(!info) {
        info = m_manager->getLibraryBook(bookID);
        if(info)
            info = LibraryBook::Ptr(info->clone());
    }

    return info;
}

void LibraryBookManagerWidget::on_tabWidget_currentChanged(int index)
{
    if(index == 2) {
        ml_return_on_fail(m_currentBook);

        if(!m_readerview->bookWidgetManager()->showBook(m_currentBook->id))
            m_readerview->openBook(m_currentBook->id);

        m_readerWidget->setEnabled(true);
    }
}
