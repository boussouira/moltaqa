#include "booklistmanagerwidget.h"
#include "ui_booklistmanagerwidget.h"
#include "mainwindow.h"
#include "selectcatdialog.h"
#include "booklistmanager.h"
#include "modelutils.h"
#include "modelenums.h"
#include "utils.h"

#include <qabstractitemmodel.h>
#include <qmessagebox.h>
#include <qdebug.h>
#include <qmenu.h>
#include <qaction.h>
#include <qinputdialog.h>

BookListManagerWidget::BookListManagerWidget(QWidget *parent) :
    ControlCenterWidget(parent),
    m_model(0),
    ui(new Ui::BookListManagerWidget)
{
    ui->setupUi(this);

    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    m_manager = LibraryManager::instance()->bookListManager();

    updateActions();

    connect(ui->toolAddCat, SIGNAL(clicked()), SLOT(addCat()));
    connect(ui->toolRemoveCat, SIGNAL(clicked()), SLOT(removeCat()));
    connect(ui->toolMoveUp, SIGNAL(clicked()), SLOT(moveUp()));
    connect(ui->toolMoveDown, SIGNAL(clicked()), SLOT(moveDown()));
    connect(ui->toolMoveRight, SIGNAL(clicked()), SLOT(moveRight()));
    connect(ui->toolMoveLeft, SIGNAL(clicked()), SLOT(moveLeft()));

    connect(ui->treeView, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(menuRequested(QPoint)));
}

BookListManagerWidget::~BookListManagerWidget()
{
    ml_delete_check(m_model);

    delete ui;
}

QString BookListManagerWidget::title()
{
    return tr("لائحة الكتب");
}

void BookListManagerWidget::loadModel()
{
    m_model = Utils::Model::cloneModel(m_manager->bookListModel());

    ui->treeView->setModel(m_model);
    ui->treeView->resizeColumnToContents(0);

    connect(m_model, SIGNAL(layoutChanged()), SLOT(updateActions()));
    connect(m_model, SIGNAL(layoutChanged()), SLOT(modelEdited()));
    connect(ui->treeView->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(updateActions()));
}

void BookListManagerWidget::save()
{
    m_manager->save(m_model);
}

void BookListManagerWidget::copyNode()
{
    QModelIndex index = Utils::Model::selectedIndex(ui->treeView);
    ml_return_on_fail(index.isValid());

    if(m_copiedItems.isEmpty()) {
        QStandardItem *item = Utils::Model::itemFromIndex(m_model, index);
        if(item)
            m_copiedItems = Utils::Model::cloneItem(item,
                                             Utils::Model::itemFromIndex(m_model, index.parent()),
                                             m_model->columnCount());
    }
}

void BookListManagerWidget::cutNode()
{
    QModelIndex index = Utils::Model::selectedIndex(ui->treeView);
    ml_return_on_fail(index.isValid());

    if(m_copiedItems.isEmpty()) {
        QStandardItem *parentItem = Utils::Model::itemFromIndex(m_model, index.parent());
        if(parentItem) {
            m_copiedItems = parentItem->takeRow(index.row());

            //ui->treeView->expand(index);
        }
    }
}

void BookListManagerWidget::pastNode()
{
    QModelIndex index = Utils::Model::selectedIndex(ui->treeView);
    ml_return_on_fail(index.isValid());

    if(m_copiedItems.size()) {
        QStandardItem *item = m_model->itemFromIndex(index);

        item->appendRow(m_copiedItems);

        Utils::Model::selectIndex(ui->treeView, index.child(item->rowCount()-1, 0));

        m_copiedItems.clear();
    }
}

void BookListManagerWidget::pastSublingNode()
{
    QModelIndex index = Utils::Model::selectedIndex(ui->treeView);
    ml_return_on_fail(index.isValid());

    if(m_copiedItems.size()) {
        QStandardItem *parentItem = Utils::Model::itemFromIndex(m_model, index.parent());
        parentItem->insertRow(index.row()+1, m_copiedItems);

        Utils::Model::selectIndex(ui->treeView, index.sibling(index.row()+1, 0));

        m_copiedItems.clear();
    }
}

void BookListManagerWidget::moveUp()
{
    Utils::Model::moveUp(m_model, ui->treeView);
}

void BookListManagerWidget::moveDown()
{
    Utils::Model::moveDown(m_model, ui->treeView);
}

void BookListManagerWidget::moveRight()
{
    Utils::Model::moveRight(m_model, ui->treeView);
}

void BookListManagerWidget::moveLeft()
{
    Utils::Model::moveLeft(m_model, ui->treeView);
}

void BookListManagerWidget::addCat()
{
    QModelIndex index = Utils::Model::selectedIndex(ui->treeView);
    QStandardItem *parentItem = Utils::Model::itemFromIndex(m_model, index.parent());

    bool ok = false;
    QString title = QInputDialog::getText(this, tr("اسم القسم"),
                                         tr("ادخل اسم القسم:"), QLineEdit::Normal,
                                         QString(), &ok);
    if(ok && title.size()) {
        QStandardItem *catItem = new QStandardItem();
        catItem->setText(title);
        catItem->setIcon(QIcon(":/images/book-cat.png"));
        catItem->setData(m_manager->getNewCategorieID(),
                         ItemRole::idRole);
        catItem->setData(ItemType::CategorieItem, ItemRole::itemTypeRole);

        parentItem->insertRow(index.row()+1, catItem);

        Utils::Model::selectIndex(ui->treeView, index.sibling(index.row()+1, 0));
    }
}

void BookListManagerWidget::removeCat()
{
    QModelIndex index = Utils::Model::selectedIndex(ui->treeView);
    ml_return_on_fail(index.isValid());

    QStandardItem *item = Utils::Model::itemFromIndex(m_model, index);

    int rep = QMessageBox::question(this,
                                title(),
                                tr("هل انت متأكد من أنك تريد حذف '%1'").arg(item->text()),
                                QMessageBox::Yes|QMessageBox::No, QMessageBox::No);


    QStandardItem *parentItem = Utils::Model::itemFromIndex(m_model, index.parent());
    if(rep == QMessageBox::Yes && parentItem) {
        parentItem->removeRow(index.row());
    }
}

void BookListManagerWidget::menuRequested(QPoint)
{
    QMenu menu(this);
    QAction *copyAct = new QAction(tr("نسخ"), &menu);
    QAction *cutAct = new QAction(tr("قص"), &menu);
    QAction *pastAct = new QAction(tr("لصق"), &menu);
    QAction *pastSublingAct = new QAction(tr("لصق في نفس المستوى"), &menu);

    cutAct->setEnabled(m_copiedItems.isEmpty());
    pastAct->setEnabled(!m_copiedItems.isEmpty());
    pastSublingAct->setEnabled(!m_copiedItems.isEmpty());

    menu.addAction(copyAct);
    menu.addAction(cutAct);
    menu.addAction(pastAct);
    menu.addAction(pastSublingAct);

    QAction *ret = menu.exec(QCursor::pos());
    if(ret) {
        if(ret == cutAct)
            cutNode();
        else if(ret == copyAct)
            copyNode();
        else if(ret == pastAct)
            pastNode();
        else if(ret == pastSublingAct)
            pastSublingNode();
    }
}

void BookListManagerWidget::updateActions()
{
    ml_return_on_fail(m_model);

    QModelIndex index = Utils::Model::selectedIndex(ui->treeView);
    QModelIndex prevIndex = index.sibling(index.row()+1, index.column());
    QModelIndex nextIndex = index.sibling(index.row()-1, index.column());

    ui->toolMoveUp->setEnabled(nextIndex.isValid());
    ui->toolMoveDown->setEnabled(prevIndex.isValid());
    ui->toolRemoveCat->setEnabled(index.isValid());
    ui->toolMoveRight->setEnabled(index.parent().isValid());
    ui->toolMoveLeft->setEnabled(index.sibling(index.row()-1, 0).isValid());
}

void BookListManagerWidget::modelEdited()
{
    setModified(true);
}
