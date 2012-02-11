#include "librarybookManagerwidget.h"
#include "ui_librarybookManagerwidget.h"
#include "mainwindow.h"
#include "librarymanager.h"
#include "librarybook.h"
#include "selectauthordialog.h"
#include "librarybookmanager.h"
#include "modelenums.h"

#include <qdebug.h>
#include <qlineedit.h>
#include <qtextedit.h>

LibraryBookManagerWidget::LibraryBookManagerWidget(QWidget *parent) :
    ControlCenterWidget(parent),
    ui(new Ui::LibraryBookManagerWidget),
    m_libraryManager(MW->libraryManager()),
    m_currentBook(0),
    m_model(0)
{
    ui->setupUi(this);

    enableEditWidgets(false);
    loadModel();
    setupActions();
}

LibraryBookManagerWidget::~LibraryBookManagerWidget()
{
    if(m_model)
        delete m_model;

    delete ui;
}

void LibraryBookManagerWidget::setupActions()
{
     foreach(QLineEdit *edit, findChildren<QLineEdit *>()) {
         connect(edit, SIGNAL(textChanged(QString)), SLOT(editted()));
     }

     foreach(QTextEdit *edit, findChildren<QTextEdit *>()) {
         connect(edit, SIGNAL(textChanged()), SLOT(editted()));
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
    m_model = m_libraryManager->bookManager()->getModel();

    ui->treeView->setModel(m_model);
    ui->treeView->resizeColumnToContents(0);
}

void LibraryBookManagerWidget::editted()
{
    if(m_currentBook) {
        m_editedBookInfo[m_currentBook->bookID] = m_currentBook;

        setModified(true);
    }
}

void LibraryBookManagerWidget::save()
{
    saveCurrentBookInfo();

    if(!m_editedBookInfo.isEmpty()) {
        m_libraryManager->bookManager()->beginUpdate();

        foreach(LibraryBook *book, m_editedBookInfo.values()) {
            qDebug("Saving book %d...", book->bookID);
            m_libraryManager->bookManager()->updateBook(book);
        }

        m_libraryManager->bookManager()->endUpdate();

        qDeleteAll(m_editedBookInfo);
        m_editedBookInfo.clear();
        m_currentBook = 0;

        setModified(false);
    }
}

void LibraryBookManagerWidget::beginEdit()
{
}

void LibraryBookManagerWidget::on_treeView_doubleClicked(const QModelIndex &index)
{
    int bookID = index.data(ItemRole::idRole).toInt();
    LibraryBook *info = getBookInfo(bookID);
    if(info) {
        saveCurrentBookInfo();
        m_currentBook = 0; // Block from emit edited() signal

        ui->lineDisplayName->setText(info->bookDisplayName);
        ui->lineAuthorName->setText(info->authorName);
        ui->plainBookInfo->setPlainText(info->bookInfo);
        ui->plainBookNames->setText(info->bookOtherNames.replace(';', '\n'));
        ui->lineEdition->setText(info->bookEdition);
        ui->lineMohaqeq->setText(info->bookMohaqeq);
        ui->linePublisher->setText(info->bookPublisher);

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

void LibraryBookManagerWidget::setupEdit(LibraryBook *info)
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
        m_currentBook->bookDisplayName = ui->lineDisplayName->text().simplified();
        m_currentBook->authorName = ui->lineAuthorName->text().simplified();
        m_currentBook->bookInfo = ui->plainBookInfo->toPlainText();
        m_currentBook->bookOtherNames = ui->plainBookNames->toPlainText().replace('\n', ';');
        m_currentBook->bookEdition = ui->lineEdition->text().simplified();
        m_currentBook->bookMohaqeq = ui->lineMohaqeq->text().simplified();
        m_currentBook->bookPublisher = ui->linePublisher->text().simplified();
    }
}

LibraryBook *LibraryBookManagerWidget::getBookInfo(int bookID)
{
    LibraryBook *info = m_editedBookInfo.value(bookID, 0);
    if(info) {
        return info;
    } else {
        info = m_libraryManager->bookManager()->getLibraryBook(bookID);
        return info->clone();
    }
}
