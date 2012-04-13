#include "librarybookmanagerwidget.h"
#include "ui_librarybookmanagerwidget.h"
#include "librarymanager.h"
#include "librarybook.h"
#include "selectauthordialog.h"
#include "librarybookmanager.h"
#include "modelenums.h"
#include "utils.h"

#include <qdebug.h>
#include <qlineedit.h>
#include <qtextedit.h>

LibraryBookManagerWidget::LibraryBookManagerWidget(QWidget *parent) :
    ControlCenterWidget(parent),
    ui(new Ui::LibraryBookManagerWidget),
    m_currentBook(0),
    m_model(0)
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

void LibraryBookManagerWidget::setupActions()
{
     foreach(QLineEdit *edit, findChildren<QLineEdit *>()) {
         connect(edit, SIGNAL(textChanged(QString)), SLOT(infoChanged()));
     }

     foreach(QTextEdit *edit, findChildren<QTextEdit *>()) {
         connect(edit, SIGNAL(textChanged()), SLOT(infoChanged()));
     }
}

void LibraryBookManagerWidget::enableEditWidgets(bool enable)
{
    ui->groupBox->setEnabled(enable);
    ui->tabWidget->setEnabled(enable);
    ui->plainBookNames->setEnabled(enable);
}

void LibraryBookManagerWidget::loadModel()
{
    m_model = m_manager->getModel();

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
        ui->plainBookInfo->setPlainText(info->info);
        ui->plainBookNames->setText(info->otherTitles.replace(';', '\n'));
        ui->lineEdition->setText(info->edition);
        ui->lineMohaqeq->setText(info->mohaqeq);
        ui->linePublisher->setText(info->publisher);

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
    ui->lineAuthorName->setEnabled(!info->isQuran());
    ui->toolChangeAuthor->setEnabled(!info->isQuran());
    ui->plainBookNames->setEnabled(!info->isQuran());
    ui->lineEdition->setEnabled(!info->isQuran());
    ui->lineMohaqeq->setEnabled(!info->isQuran());
    ui->linePublisher->setEnabled(!info->isQuran());
}

void LibraryBookManagerWidget::saveCurrentBookInfo()
{
    if(m_currentBook) {
        m_currentBook->title = ui->lineDisplayName->text().simplified();
        m_currentBook->authorName = ui->lineAuthorName->text().simplified();
        m_currentBook->info = ui->plainBookInfo->toPlainText();
        m_currentBook->otherTitles = ui->plainBookNames->toPlainText().replace('\n', ';');
        m_currentBook->edition = ui->lineEdition->text().simplified();
        m_currentBook->mohaqeq = ui->lineMohaqeq->text().simplified();
        m_currentBook->publisher = ui->linePublisher->text().simplified();
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
