#include "selectcatdialog.h"
#include "ui_selectcatdialog.h"
#include "librarymanager.h"
#include "modelviewfilter.h"
#include "booklistmanager.h"
#include "modelenums.h"
#include "modelutils.h"
#include "utils.h"
#include "sortfilterproxymodel.h"

#include <qstandarditemmodel.h>
#include <qmessagebox.h>

selectCatDialog::selectCatDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::selectCatDialog)
{
    ui->setupUi(this);

    m_libraryManager = LibraryManager::instance();

    BookListManager *bookListManager = m_libraryManager->bookListManager();
    m_model = Utils::Model::cloneModel(bookListManager->catListModel());

    m_filter = new ModelViewFilter(this);
    m_filter->setSourceModel(m_model);
    m_filter->setTreeView(ui->treeView);
    m_filter->setLineEdit(ui->lineSearch);
    m_filter->setup();

    m_selectedItem = 0;

    connect(ui->pushSelect, SIGNAL(clicked()), SLOT(selectCat()));
    connect(ui->pushCancel, SIGNAL(clicked()), SLOT(cancel()));
    connect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)), SLOT(selectCat()));
}

selectCatDialog::~selectCatDialog()
{
    ml_delete_check(m_model);
    ml_delete_check(m_filter);
    delete ui;
}

void selectCatDialog::selectCat()
{
    QModelIndex index = Utils::Model::selectedIndex(ui->treeView);

    if(!index.isValid()) {
        QMessageBox::warning(this,
                             tr("اختيار قسم"),
                             tr("لم تقم باختيار اي قسم"));
        return;
    }

    m_selectedItem = m_model->itemFromIndex(m_filter->filterModel()->mapToSource(index));

    emit catSelected();
    accept();
}

void selectCatDialog::cancel()
{
    reject();
}

QStandardItem *selectCatDialog::selectedNode()
{
    return m_selectedItem;
}

QString selectCatDialog::selectedCatName()
{
    return m_selectedItem->text();
}

int selectCatDialog::selectedCatID()
{
    return m_selectedItem->data(ItemRole::idRole).toInt();
}
