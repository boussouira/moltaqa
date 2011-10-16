#include "selectcatdialog.h"
#include "ui_selectcatdialog.h"
#include "mainwindow.h"
#include "librarymanager.h"
#include "bookslistmodel.h"
#include "bookslistnode.h"
#include "sortfilterproxymodel.h"

#include <qmessagebox.h>

selectCatDialog::selectCatDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::selectCatDialog)
{
    ui->setupUi(this);

    m_libraryManager = MW->libraryManager();
    m_model = m_libraryManager->catsListModel();

    m_filter = new SortFilterProxyModel(this);
    m_filter->setSourceModel(m_model);

    m_selectedNode = 0;

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
    if(ui->treeView->selectionModel()->selectedIndexes().isEmpty()) {
        QMessageBox::warning(this,
                             tr("اختيار قسم"),
                             tr("لم تقم باختيار اي قسم"));
        return;
    }

    m_selectedNode = m_model->nodeFromIndex(m_filter->mapToSource(ui->treeView->selectionModel()->selectedIndexes().first()));

    emit catSelected();
    accept();
}

void selectCatDialog::cancel()
{
    reject();
}

BooksListNode *selectCatDialog::selectedNode()
{
    return m_selectedNode;
}

QString selectCatDialog::selectedCatName()
{
    return m_selectedNode->title;
}

int selectCatDialog::selectedCatID()
{
    return m_selectedNode->id;
}
