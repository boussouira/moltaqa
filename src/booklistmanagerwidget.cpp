#include "booklistmanagerwidget.h"
#include "ui_booklistmanagerwidget.h"
#include "mainwindow.h"
#include "selectcatdialog.h"
#include "booklistmanager.h"
#include "modelutils.h"
#include "modelenums.h"

#include <QModelIndex>
#include <qmessagebox.h>
#include <qdebug.h>
#include <qmenu.h>
#include <qaction.h>
#include <qinputdialog.h>

BookListManagerWidget::BookListManagerWidget(QWidget *parent) :
    ControlCenterWidget(parent),
    ui(new Ui::BookListManagerWidget)
{
    ui->setupUi(this);

    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_model = 0;

    loadModel();
    updateActions();

    connect(ui->toolAddCat, SIGNAL(clicked()), SLOT(addCat()));
    connect(ui->toolRemoveCat, SIGNAL(clicked()), SLOT(removeCat()));
    connect(ui->toolMoveUp, SIGNAL(clicked()), SLOT(moveUp()));
    connect(ui->toolMoveDown, SIGNAL(clicked()), SLOT(moveDown()));
    connect(ui->toolMoveRight, SIGNAL(clicked()), SLOT(moveRight()));
    connect(ui->toolMoveLeft, SIGNAL(clicked()), SLOT(moveLeft()));
    connect(ui->treeView, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(menuRequested(QPoint)));
    connect(ui->treeView->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(updateActions()));
}

BookListManagerWidget::~BookListManagerWidget()
{
    if(m_model)
        delete m_model;

    delete ui;
}

QString BookListManagerWidget::title()
{
    return tr("لائحة الكتب");
}

void BookListManagerWidget::loadModel()
{
    m_model = Utils::cloneModel(m_libraryManager->bookListManager()->bookListModel());

    ui->treeView->setModel(m_model);
    ui->treeView->resizeColumnToContents(0);

    connect(m_model, SIGNAL(layoutChanged()), SLOT(updateActions()));
    connect(m_model, SIGNAL(layoutChanged()), SLOT(modelEdited()));
}

void BookListManagerWidget::save()
{
    m_libraryManager->bookListManager()->save(m_model);
}

void BookListManagerWidget::beginEdit()
{
}

void BookListManagerWidget::cutNode()
{
    QModelIndex index = Utils::selectedIndex(ui->treeView);
    if(!index.isValid())
        return;

    if(m_copiedItems.isEmpty()) {
        QStandardItem *parentItem = Utils::itemFromIndex(m_model, index.parent());
        if(parentItem) {
            m_copiedItems = parentItem->takeRow(index.row());

            //ui->treeView->expand(index);
        }
    }
}

void BookListManagerWidget::pastNode()
{
    QModelIndex index = Utils::selectedIndex(ui->treeView);
    if(!index.isValid())
        return;

    if(!m_copiedItems.isEmpty()) {
        QStandardItem *item = m_model->itemFromIndex(index);

        item->appendRow(m_copiedItems);

        Utils::selectIndex(ui->treeView, index.child(item->rowCount()-1, 0));

        m_copiedItems.clear();
    }
}

void BookListManagerWidget::pastSublingNode()
{
    QModelIndex index = Utils::selectedIndex(ui->treeView);
    if(!index.isValid())
        return;

    if(!m_copiedItems.isEmpty()) {
        QStandardItem *parentItem = Utils::itemFromIndex(m_model, index.parent());
        parentItem->insertRow(index.row()+1, m_copiedItems);

        Utils::selectIndex(ui->treeView, index.sibling(index.row()+1, 0));

        m_copiedItems.clear();
    }
}

void BookListManagerWidget::moveUp()
{
    Utils::moveUp(m_model, ui->treeView);
}

void BookListManagerWidget::moveDown()
{
    Utils::moveDown(m_model, ui->treeView);
}

void BookListManagerWidget::moveRight()
{
    Utils::moveRight(m_model, ui->treeView);
}

void BookListManagerWidget::moveLeft()
{
    Utils::moveLeft(m_model, ui->treeView);
}

void BookListManagerWidget::addCat()
{
    QModelIndex index = Utils::selectedIndex(ui->treeView);
    QStandardItem *parentItem = Utils::itemFromIndex(m_model, index.parent());

    bool ok = false;
    QString title = QInputDialog::getText(this, tr("اسم القسم"),
                                         tr("ادخل اسم القسم:"), QLineEdit::Normal,
                                         QString(), &ok);
    if(ok && !title.isEmpty()) {
        QStandardItem *catItem = new QStandardItem();
        catItem->setText(title);
        catItem->setIcon(QIcon(":/images/book-cat.png"));
        catItem->setData(m_libraryManager->bookListManager()->maxCategorieID(),
                         ItemRole::idRole);
        catItem->setData(ItemType::CategorieItem, ItemRole::itemTypeRole);

        parentItem->insertRow(index.row()+1, catItem);

        Utils::selectIndex(ui->treeView, index.sibling(index.row()+1, 0));
    }
}

void BookListManagerWidget::removeCat()
{
    QModelIndex index = Utils::selectedIndex(ui->treeView);
    if(!index.isValid())
        return;

    QStandardItem *item = Utils::itemFromIndex(m_model, index);

    if(item->hasChildren()) {
        QMessageBox::warning(this,
                             tr("حذف القسم"),
                             tr("يجب ان تحذف كل الاقسام الفرعية لقسم '%1' قبل حذفه"
                                "\n"
                                "ويجب نقل الكتب الموجودة في هذا القسم الى قسم اخر").arg(item->text()));
        return;
    }

    int rep;
    rep = QMessageBox::question(this,
                                tr("حذف قسم"),
                                tr("هل انت متأكد من أنك تريد حذف '%1'").arg(item->text()),
                                QMessageBox::Yes|QMessageBox::No, QMessageBox::No);

    QStandardItem *parentItem = Utils::itemFromIndex(m_model, index.parent());
    if(rep == QMessageBox::Yes) {
        parentItem->removeRow(index.row());
    }
}

void BookListManagerWidget::menuRequested(QPoint)
{
    QMenu menu(this);
    QAction *cutAct = new QAction(tr("قص"), &menu);
    QAction *pastAct = new QAction(tr("لصق"), &menu);
    QAction *pastSublingAct = new QAction(tr("لصق في نفس المستوى"), &menu);

    cutAct->setEnabled(m_copiedItems.isEmpty());
    pastAct->setEnabled(!m_copiedItems.isEmpty());
    pastSublingAct->setEnabled(!m_copiedItems.isEmpty());

    menu.addAction(cutAct);
    menu.addAction(pastAct);
    menu.addAction(pastSublingAct);

    QAction *ret = menu.exec(QCursor::pos());
    if(ret) {
        if(ret == cutAct)
            cutNode();
        else if(ret == pastAct)
            pastNode();
        else if(ret == pastSublingAct)
            pastSublingNode();
    }
}

void BookListManagerWidget::updateActions()
{
    if(!m_model)
        return;

    QModelIndex index = Utils::selectedIndex(ui->treeView);
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
