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

    enableEditWidgets(false);
    loadModel();
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
    m_model = LibraryBookManager::instance()->getModel();

    ui->treeView->setModel(m_model);
    ui->treeView->resizeColumnToContents(0);
}

void LibraryBookManagerWidget::infoChanged()
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
        LibraryBookManager::instance()->beginUpdate();

        foreach(LibraryBook *book, m_editedBookInfo.values()) {
            qDebug("Saving book %d...", book->bookID);
            LibraryBookManager::instance()->updateBook(book);
        }

        LibraryBookManager::instance()->endUpdate();

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

        // delete current book if it's not in the edited books
        if(m_currentBook && !m_editedBookInfo.contains(m_currentBook->bookID))
            delete m_currentBook;

        // this will cause infoChanged() to ignore changes that we will made next
        m_currentBook = 0;

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
    LibraryBook *info = m_editedBookInfo.value(bookID);
    if(!info) {
        info = LibraryBookManager::instance()->getLibraryBook(bookID);
        if(info)
            info = info->clone();
    }

    return info;
}
