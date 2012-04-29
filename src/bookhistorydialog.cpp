#include "bookhistorydialog.h"
#include "ui_bookhistorydialog.h"
#include "librarymanager.h"
#include "utils.h"
#include "modelenums.h"

BookHistoryDialog::BookHistoryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BookHistoryDialog)
{
    ui->setupUi(this);
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
