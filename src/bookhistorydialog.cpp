#include "bookhistorydialog.h"
#include "ui_bookhistorydialog.h"
#include "librarymanager.h"
#include "utils.h"
#include "modelenums.h"
#include "checkablemessagebox.h"

#include <qmessagebox.h>

BookHistoryDialog::BookHistoryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BookHistoryDialog)
{
    ui->setupUi(this);

    connect(ui->pushDeleteHistory, SIGNAL(clicked()), SLOT(deleteBookHistory()));
}

BookHistoryDialog::~BookHistoryDialog()
{
    delete ui;
}

void BookHistoryDialog::setup()
{
    ml_return_on_fail2(m_book, "BookHistoryDialog::setup book is null");

    m_model = LibraryManager::instance()->bookManager()->getBookHistoryModel(m_book->id);
    ui->treeView->setModel(m_model.data());

    setWindowTitle(m_book->title);
}

void BookHistoryDialog::on_treeView_doubleClicked(const QModelIndex &index)
{
    int page = index.sibling(index.row(), 0).data(ItemRole::idRole).toInt();

    if(page)
        emit openPage(page);
}

void BookHistoryDialog::deleteBookHistory()
{
    ml_return_on_fail2(m_book, "BookHistoryDialog::setup book is null");

    int ret = CheckableMessageBox::question(this,
                                            tr("حذف تاريخ تصفح الكتاب"),
                                            tr("هل تريد حذف تاريخ تصفح كتاب '%1'؟").arg(m_book->title),
                                            "CheckableMessageBox/deleteBookHistory",
                                            QDialogButtonBox::Yes);

    if(ret == QDialogButtonBox::Yes) {
        if(LibraryManager::instance()->bookManager()->deleteBookHistory(m_book->id)) {
            setup();
        } else {
            QMessageBox::warning(this,
                                 tr("حذف تاريخ تصفح الكتاب"),
                                 tr("حدث خطأ أثناء حذف تاريخ تصفح الكتاب"));
        }
    }
}
