#include "searchfieldsdialog.h"
#include "ui_searchfieldsdialog.h"
#include "librarymanager.h"
#include "searchmanager.h"

#include <qinputdialog.h>
#include <qmessagebox.h>

SearchFieldsDialog::SearchFieldsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchFieldsDialog)
{
    ui->setupUi(this);

    m_searchManager = LibraryManager::instance()->searchManager();

    loadSearchfields();
}

SearchFieldsDialog::~SearchFieldsDialog()
{
    delete ui;
}

void SearchFieldsDialog::loadSearchfields()
{
    ui->listWidget->clear();

    foreach(SearchFieldInfo field, m_searchManager->getFieldNames()) {
        QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
        item->setText(field.name);
        item->setData(Qt::UserRole, field.fieldID);

        ui->listWidget->addItem(item);
    }
}

void SearchFieldsDialog::on_toolEdit_clicked()
{
    if(ui->listWidget->selectedItems().isEmpty()) {
        QMessageBox::warning(this,
                             windowTitle(),
                             tr("لم تقم باختيار اي مجال بحث"));
        return;
    }

    QListWidgetItem *item = ui->listWidget->selectedItems().first();

    QString name = QInputDialog::getText(this,
                                         windowTitle(),
                                         tr("اسم مجال البحث:"),
                                         QLineEdit::Normal,
                                         item->text()).trimmed();
    if(!name.isEmpty()) {
        m_searchManager->setFieldName(name, item->data(Qt::UserRole).toInt());
        loadSearchfields();
    }
}

void SearchFieldsDialog::on_toolDelete_clicked()
{
    if(ui->listWidget->selectedItems().isEmpty()) {
        QMessageBox::warning(this,
                             windowTitle(),
                             tr("لم تقم باختيار اي مجال بحث"));
        return;
    }

    QListWidgetItem *item = ui->listWidget->selectedItems().first();
    m_searchManager->removeField(item->data(Qt::UserRole).toInt());

    loadSearchfields();
}
