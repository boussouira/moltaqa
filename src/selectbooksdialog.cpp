#include "selectbooksdialog.h"
#include "ui_selectbooksdialog.h"
#include "librarymanager.h"
#include "modelviewfilter.h"
#include "modelenums.h"
#include "modelutils.h"
#include "utils.h"

#include <qstandarditemmodel.h>
#include <qmessagebox.h>

selectBooksDialog::selectBooksDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::selectBooksDialog)
{
    ui->setupUi(this);

    m_model = LibraryManager::instance()->bookManager()->getModel(false);
    Utils::Model::setModelCheckable(m_model.data());

    m_filter = new ModelViewFilter(this);
    m_filter->addFilterColumn(0, Qt::DisplayRole, tr("عنوان الكتاب"));
    m_filter->addFilterColumn(1, Qt::DisplayRole, tr("اسم المؤلف"));
    m_filter->setSourceModel(m_model.data());
    m_filter->setTreeView(ui->treeView);
    m_filter->setLineEdit(ui->lineSearch);
    m_filter->setup();

    connect(ui->pushSelect, SIGNAL(clicked()), SLOT(selectBooks()));
    connect(ui->pushCancel, SIGNAL(clicked()), SLOT(cancel()));
}

selectBooksDialog::~selectBooksDialog()
{
    ml_delete_check(m_filter);
    delete ui;
}

void selectBooksDialog::selectBooks()
{
    m_selectedBooks.clear();
    m_filter->clearFilter();

    QModelIndex index = m_filter->filterModel()->index(0, 0);
    while(index.isValid()) {
        if(index.data(Qt::CheckStateRole).toInt() == Qt::Checked)
            m_selectedBooks.append(index.data(ItemRole::idRole).toInt());

        index = index.sibling(index.row()+1, 0);
    }

    if(m_selectedBooks.size()) {
        accept();
    } else {
        QMessageBox::warning(this,
                             tr("اختيار كتاب"),
                             tr("لم تقم باختيار اي كتاب"));
    }
}

void selectBooksDialog::cancel()
{
    reject();
}
