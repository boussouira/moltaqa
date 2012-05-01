#include "selectauthordialog.h"
#include "ui_selectauthordialog.h"
#include "mainwindow.h"
#include "librarymanager.h"
#include "sortfilterproxymodel.h"
#include "authorsmanager.h"
#include "modelenums.h"
#include "modelutils.h"

#include <qstandarditemmodel.h>
#include <qmessagebox.h>

selectAuthorDialog::selectAuthorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::selectAuthorDialog)
{
    ui->setupUi(this);

    m_model = LibraryManager::instance()->authorsManager()->authorsModel();

    m_filter = new SortFilterProxyModel(this);
    m_filter->setSourceModel(m_model);

    ui->treeView->setModel(m_filter);
    ui->treeView->resizeColumnToContents(0);

    m_authorID = 0;

    connect(ui->lineSearch, SIGNAL(textChanged(QString)), m_filter, SLOT(setArabicFilterRegexp(QString)));
    connect(ui->pushSelect, SIGNAL(clicked()), SLOT(selectAuthor()));
    connect(ui->pushCancel, SIGNAL(clicked()), SLOT(cancel()));
}

selectAuthorDialog::~selectAuthorDialog()
{
    delete ui;
}

void selectAuthorDialog::selectAuthor()
{
    QModelIndex index = Utils::Model::selectedIndex(ui->treeView);
    if(index.isValid()) {
        m_authorName = index.data().toString();
        m_authorID = index.data(ItemRole::authorIdRole).toInt();

        emit authorSelected();

        accept();
    }
}

void selectAuthorDialog::cancel()
{
    reject();
}

QString selectAuthorDialog::selectedAuthorName()
{
    return m_authorName;
}

int selectAuthorDialog::selectedAuthorID()
{
    return m_authorID;
}

void selectAuthorDialog::on_treeView_doubleClicked(const QModelIndex &index)
{
    if(index.isValid())
        selectAuthor();
}
