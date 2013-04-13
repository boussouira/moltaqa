#include "selectauthordialog.h"
#include "ui_selectauthordialog.h"
#include "authorsmanager.h"
#include "librarymanager.h"
#include "modelenums.h"
#include "modelutils.h"
#include "modelviewfilter.h"
#include "utils.h"

#include <qmessagebox.h>
#include <qstandarditemmodel.h>

selectAuthorDialog::selectAuthorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::selectAuthorDialog)
{
    ui->setupUi(this);

    m_model = LibraryManager::instance()->authorsManager()->authorsModel();

    m_filter = new ModelViewFilter(this);
    m_filter->setSourceModel(m_model);
    m_filter->setTreeView(ui->treeView);
    m_filter->setLineEdit(ui->lineSearch);
    m_filter->setup();

    m_authorID = 0;

    connect(ui->pushSelect, SIGNAL(clicked()), SLOT(selectAuthor()));
    connect(ui->pushCancel, SIGNAL(clicked()), SLOT(cancel()));
}

selectAuthorDialog::~selectAuthorDialog()
{
    ml_delete_check(m_model);
    ml_delete_check(m_filter);
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
