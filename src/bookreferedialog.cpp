#include "bookreferedialog.h"
#include "ui_bookreferedialog.h"
#include "ui_addbookreferdialog.h"
#include "librarymanager.h"
#include "modelenums.h"
#include "modelutils.h"
#include "utils.h"

#include <qstandarditemmodel.h>
#include <qlabel.h>
#include <qmessagebox.h>

static int ReferTextRole = Qt::UserRole + 100;

BookRefereDialog::BookRefereDialog(QWidget *parent) :
    QDialog(parent),
    m_model(0),
    ui(new Ui::BookRefereDialog)
{
    ui->setupUi(this);

    loadModel();
}

BookRefereDialog::~BookRefereDialog()
{
    ml_delete_check(m_model);
    delete ui;
}

void BookRefereDialog::loadModel()
{
    ml_delete_check(m_model);

    m_model = new QStandardItemModel(this);

    QHashIterator<int, QAction *> i(LibraryManager::instance()->textRefersActions());
    while(i.hasNext()){
        i.next();

        QStandardItem *item = new QStandardItem(i.value()->text());
        item->setData(i.key(), ItemRole::idRole);
        item->setData(i.value()->data(), ReferTextRole);

        m_model->appendRow(item);
    }

    ui->treeView->setModel(m_model);
}

void BookRefereDialog::on_toolAdd_clicked()
{
    Ui::AddBookReferDialog ui2;
    QDialog dialog(this);
    ui2.setupUi(&dialog);

    if(dialog.exec() == QDialog::Accepted) {
        QString name = ui2.lineEdit->text().trimmed();
        QString refer = ui2.textEdit->toPlainText();

        ml_return_on_fail2(name.size(), "BookRefereDialog: name is empty");
        ml_return_on_fail2(refer.size(), "BookRefereDialog: refer text is empty");

        LibraryManager::instance()->addTextRefers(name, refer);

        loadModel();
    }
}

void BookRefereDialog::on_toolEdit_clicked()
{
    QModelIndex index = Utils::Model::selectedIndex(ui->treeView);
    if(index.isValid()) {
        int rid = index.data(ItemRole::idRole).toInt();
        QString name = index.data(Qt::DisplayRole).toString();
        QString referText = index.data(ReferTextRole).toString();

        Ui::AddBookReferDialog ui2;
        QDialog dialog(this);
        ui2.setupUi(&dialog);

        ui2.lineEdit->setText(name);
        ui2.textEdit->setPlainText(referText);

        if(dialog.exec() == QDialog::Accepted) {
            name = ui2.lineEdit->text().trimmed();
            referText = ui2.textEdit->toPlainText();

            LibraryManager::instance()->editRefers(rid, name, referText);
            loadModel();
        }
    } else {
        QMessageBox::warning(this,
                             tr("تعديل العزو"),
                             tr("لم تقم باختيار اي عزو"));
    }
}

void BookRefereDialog::on_toolDelete_clicked()
{
    QModelIndex index = Utils::Model::selectedIndex(ui->treeView);
    if(index.isValid()) {
        int rid = index.data(ItemRole::idRole).toInt();
        QString name = index.data(Qt::DisplayRole).toString();

        if(QMessageBox::question(this,
                                 tr("حذف عزو"),
                                 tr("هل انت متأكد من انك تريد حذف '%1'؟").arg(name),
                                 QMessageBox::Yes|QMessageBox::No,
                                 QMessageBox::No)==QMessageBox::Yes) {

            LibraryManager::instance()->deleteRefer(rid);
            loadModel();
        }
    } else {
        QMessageBox::warning(this,
                             tr("حذف العزو"),
                             tr("لم تقم باختيار اي عزو"));
    }
}
