#include "selectcatdialog.h"
#include "ui_selectcatdialog.h"
#include "mainwindow.h"
#include "librarymanager.h"
#include "sortfilterproxymodel.h"
#include "booklistmanager.h"
#include "modelenums.h"
#include "modelutils.h"

#include <qstandarditemmodel.h>
#include <qmessagebox.h>

selectCatDialog::selectCatDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::selectCatDialog)
{
    ui->setupUi(this);

    m_libraryManager = MW->libraryManager();
    m_model = Utils::cloneModel(MW->libraryManager()->bookListManager()->catListModel());

    m_filter = new SortFilterProxyModel(this);
    m_filter->setSourceModel(m_model);

    m_selectedItem = 0;

    ui->treeView->setModel(m_filter);
    ui->treeView->setColumnHidden(1, true);
    ui->treeView->resizeColumnToContents(0);

    connect(ui->lineSearch, SIGNAL(textChanged(QString)), m_filter, SLOT(setFilterRegExp(QString)));
    connect(ui->lineSearch, SIGNAL(textChanged(QString)), ui->treeView, SLOT(expandAll()));
    connect(ui->pushSelect, SIGNAL(clicked()), SLOT(selectCat()));
    connect(ui->pushCancel, SIGNAL(clicked()), SLOT(cancel()));
    connect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)), SLOT(selectCat()));
}

selectCatDialog::~selectCatDialog()
{
    delete ui;
    delete m_model;
}

void selectCatDialog::selectCat()
{
    QModelIndex index = Utils::selectedIndex(ui->treeView);

    if(!index.isValid()) {
        QMessageBox::warning(this,
                             tr("اختيار قسم"),
                             tr("لم تقم باختيار اي قسم"));
        return;
    }

    m_selectedItem = m_model->itemFromIndex(m_filter->mapToSource(index));

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
