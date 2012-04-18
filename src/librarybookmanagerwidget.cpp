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

#include <qdebug.h>
#include <qlineedit.h>
#include <qtextedit.h>

LibraryBookManagerWidget::LibraryBookManagerWidget(QWidget *parent) :
    ControlCenterWidget(parent),
    ui(new Ui::LibraryBookManagerWidget),
    m_currentBook(0),
    m_model(0),
    m_webEdit(0)
{
    ui->setupUi(this);

    m_manager = LibraryManager::instance()->bookManager();

    enableEditWidgets(false);
    setupActions();
}

LibraryBookManagerWidget::~LibraryBookManagerWidget()
{
    ML_DELETE_CHECK(m_model);

    delete ui;
}

QString LibraryBookManagerWidget::title()
{
    return tr("الكتب");
}

void LibraryBookManagerWidget::aboutToShow()
{
    ML_ASSERT(!m_webEdit);

    m_webEdit = new EditWebView(this);

    QVBoxLayout *layout = new QVBoxLayout(ui->tabBookInfo);
    layout->addWidget(m_webEdit);
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
}

void LibraryBookManagerWidget::enableEditWidgets(bool enable)
{
    ui->tabWidget->setEnabled(enable);
}

void LibraryBookManagerWidget::loadModel()
{
    ML_DELETE_CHECK(m_model);

    m_model = Utils::Model::cloneModel(m_manager->getModel().data());

    ui->treeView->setModel(m_model);
    ui->treeView->resizeColumnToContents(0);
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
    ML_ASSERT(m_webEdit);

    if(m_webEdit->pageModified())
        infoChanged();
}

void LibraryBookManagerWidget::save()
{
    saveCurrentBookInfo();

    if(!m_editedBookInfo.isEmpty()) {
        m_manager->transaction();

        foreach(LibraryBookPtr book, m_editedBookInfo.values()) {
            qDebug("Saving book %d...", book->id);
            m_manager->updateBook(book);
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
    int bookID = index.data(ItemRole::idRole).toInt();
    LibraryBookPtr info = getBookInfo(bookID);
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

void LibraryBookManagerWidget::setupEdit(LibraryBookPtr info)
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

LibraryBookPtr LibraryBookManagerWidget::getBookInfo(int bookID)
{
    LibraryBookPtr info = m_editedBookInfo.value(bookID);
    if(!info) {
        info = m_manager->getLibraryBook(bookID);
        if(info)
            info = LibraryBookPtr(info->clone());
    }

    return info;
}
