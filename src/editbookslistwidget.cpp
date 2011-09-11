#include "editbookslistwidget.h"
#include "ui_editbookslistwidget.h"
#include "mainwindow.h"
#include "indexdb.h"
#include "editablebookslistmodel.h"
#include "bookinfo.h"
#include "selectauthordialog.h"

#include <qdebug.h>
#include <qlineedit.h>
#include <qtextedit.h>

EditBooksListWidget::EditBooksListWidget(QWidget *parent) :
    AbstractEditWidget(parent),
    ui(new Ui::EditBooksListWidget)
{
    ui->setupUi(this);

    m_indexDB = MainWindow::mainWindow()->indexDB();
    m_booksModel = new EditableBooksListModel(this);
    m_booksModel->setRootNode(m_indexDB->booksListModel()->m_rootNode);
    m_booksModel->setModelEditibale(false);

    ui->treeView->setModel(m_booksModel);
    ui->treeView->resizeColumnToContents(0);
    ui->treeView->resizeColumnToContents(1);

    m_bookInfo = 0;

    enableEditWidgets(false);
    setupActions();
}

EditBooksListWidget::~EditBooksListWidget()
{
    delete ui;
}

void EditBooksListWidget::setupActions()
{
     foreach(QLineEdit *edit, findChildren<QLineEdit *>()) {
         connect(edit, SIGNAL(textChanged(QString)), SLOT(editted()));
     }

     foreach(QTextEdit *edit, findChildren<QTextEdit *>()) {
         connect(edit, SIGNAL(textChanged()), SLOT(editted()));
     }
}
void EditBooksListWidget::enableEditWidgets(bool enable)
{
    ui->groupBox->setEnabled(enable);
    ui->tabWidget->setEnabled(enable);
}

void EditBooksListWidget::editted()
{
    if(m_bookInfo) {
        m_editedBookInfo[m_bookInfo->bookID] = m_bookInfo;

        emit edited(true);
    }
}

void EditBooksListWidget::save()
{
    saveCurrentBookInfo();

    if(!m_editedBookInfo.isEmpty()) {
        m_indexDB->transaction();

        foreach(BookInfo *info, m_editedBookInfo.values()) {
            m_indexDB->updateBookInfo(info);
        }

        m_indexDB->commit();

        qDeleteAll(m_editedBookInfo);
        m_editedBookInfo.clear();
        m_bookInfo = 0;

        emit edited(false);
    }
}

void EditBooksListWidget::on_treeView_doubleClicked(const QModelIndex &index)
{
    BooksListNode *node = m_booksModel->nodeFromIndex(index);
    if(node->type == BooksListNode::Book) {
        BookInfo *info = getBookInfo(node->id);
        if(info) {
            saveCurrentBookInfo();
            m_bookInfo = 0; // Block from emit edited() signal

            ui->lineFullName->setText(info->bookFullName);
            ui->lineDisplayName->setText(info->bookDisplayName);
            ui->lineAuthorName->setText(info->authorName);
            ui->plainBookInfo->setPlainText(info->bookInfo);
            ui->plainBookNames->setText(info->bookOtherNames.replace(';', '\n'));
            ui->lineEdition->setText(info->bookEdition);
            ui->lineMohaqeq->setText(info->bookMohaqeq);
            ui->linePublisher->setText(info->bookPublisher);

            enableEditWidgets(true);
            m_bookInfo = info;
        }
    }
}

void EditBooksListWidget::on_toolChangeAuthor_clicked()
{
    selectAuthorDialog dialog(this);

    if(dialog.exec() == QDialog::Accepted) {
        ui->lineAuthorName->setText(dialog.selectedAuthorName());
        if(m_bookInfo) {
            m_bookInfo->authorID = dialog.selectedAuthorID();
            m_bookInfo->authorName = dialog.selectedAuthorName();
        }
    }
}

void EditBooksListWidget::saveCurrentBookInfo()
{
    if(m_bookInfo) {
        m_bookInfo->bookFullName = ui->lineFullName->text().simplified();
        m_bookInfo->bookDisplayName = ui->lineDisplayName->text().simplified();
        m_bookInfo->authorName = ui->lineAuthorName->text().simplified();
        m_bookInfo->bookInfo = ui->plainBookInfo->toPlainText();
        m_bookInfo->bookOtherNames = ui->plainBookNames->toPlainText().replace('\n', ';');
        m_bookInfo->bookEdition = ui->lineEdition->text().simplified();
        m_bookInfo->bookMohaqeq = ui->lineMohaqeq->text().simplified();
        m_bookInfo->bookPublisher = ui->linePublisher->text().simplified();
    }
}

BookInfo *EditBooksListWidget::getBookInfo(int bookID)
{
    BookInfo *info = m_editedBookInfo.value(bookID, 0);
    if(!info) {
        info = m_indexDB->getBookInfo(bookID, true);
    }

    return info;
}
