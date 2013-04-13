#include "bookreferedialog.h"
#include "ui_bookreferedialog.h"
#include "librarymanager.h"
#include "modelenums.h"
#include "modelutils.h"
#include "ui_addbookreferdialog.h"
#include "utils.h"

#include <qlabel.h>
#include <qmessagebox.h>
#include <qstandarditemmodel.h>

static int ReferTextRole = Qt::UserRole + 100;

BookRefereDialog::BookRefereDialog(QWidget *parent) :
    QDialog(parent),
    m_model(0),
    m_referEditor(0),
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

void BookRefereDialog::setupRefererEditor(Ui::AddBookReferDialog *editor)
{
    m_referEditor = editor;
    connect(editor->labelHelp, SIGNAL(linkActivated(QString)),
            SLOT(addReferToEditor(QString)));
}

void BookRefereDialog::on_toolAdd_clicked()
{
    Ui::AddBookReferDialog ui2;
    QDialog dialog(this);
    ui2.setupUi(&dialog);

    setupRefererEditor(&ui2);

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

        setupRefererEditor(&ui2);

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

void BookRefereDialog::addReferToEditor(QString href)
{
    ml_return_on_fail(m_referEditor);

    QString text;
    if(href == "#nass")
        text = tr("النص");
    else if(href == "#author")
        text = tr("المؤلف");
    else if(href == "#book")
        text = tr("الكتاب");
    else if(href == "#page")
        text = tr("الصفحة");
    else if(href == "#author")
        text = tr("المؤلف");
    else if(href == "#part")
        text = tr("الجزء");
    else if(href == "#haddit")
        text = tr("الحديث");

    if(text.size()) {
        m_referEditor->textEdit->textCursor().insertText('*' + text + '*');
        m_referEditor->textEdit->setFocus();
    }
}
