#include "bookindexeditor.h"
#include "ui_bookindexeditor.h"
#include "modelenums.h"
#include "editwebview.h"
#include "richbookreader.h"
#include "bookeditorview.h"

#include <qfile.h>
#include <qxmlstream.h>
#include <qmessagebox.h>
#include <qinputdialog.h>

BookIndexEditor::BookIndexEditor(BookEditorView *parent) :
    QWidget(parent),
    ui(new Ui::BookIndexEditor),
    m_editView(parent)
{
    ui->setupUi(this);

    connect(ui->toolAddTitle, SIGNAL(clicked()), SLOT(addTitle()));
    connect(ui->toolRemoveTitle, SIGNAL(clicked()), SLOT(removeTitle()));
    connect(ui->toolMoveUp, SIGNAL(clicked()), SLOT(moveUp()));
    connect(ui->toolMoveDown, SIGNAL(clicked()), SLOT(moveDown()));
    connect(ui->toolMoveRight, SIGNAL(clicked()), SLOT(moveRight()));
    connect(ui->toolMoveLeft, SIGNAL(clicked()), SLOT(moveLeft()));
    connect(ui->toolLinkTitle, SIGNAL(clicked()), SLOT(linkTitle()));
}

BookIndexEditor::~BookIndexEditor()
{
    delete ui;
}

void BookIndexEditor::setup()
{
    setModel(m_editView->m_bookReader->indexModel());
}

void BookIndexEditor::setModel(QStandardItemModel *model)
{
    m_model = model;
    ui->treeView->setModel(model);

    connect(ui->treeView->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(updateActions()));

    updateActions();
}

void BookIndexEditor::saveModel(QXmlStreamWriter *writer)
{
    Q_CHECK_PTR(m_model);

    writer->writeStartDocument();
    writer->writeStartElement("titles");

    for(int i=0; i<m_model->rowCount(); i++) {
        QStandardItem *item = m_model->item(i);
        writeItem(item, writer);
    }

    writer->writeEndElement();
    writer->writeEndDocument();
}

void BookIndexEditor::writeItem(QStandardItem *item, QXmlStreamWriter *writer)
{
    writer->writeStartElement("item");
    writer->writeAttribute("pageID", item->data(ItemRole::idRole).toString());
    writer->writeAttribute("text", item->text());

    if(item->hasChildren()) {
        for(int i=0; i<item->rowCount(); i++) {
            QStandardItem *child = item->child(i);
            writeItem(child, writer);
        }
    }

    writer->writeEndElement();
}

QModelIndex BookIndexEditor::selectedIndex()
{
    QModelIndexList selection = ui->treeView->selectionModel()->selectedIndexes();

    return selection.isEmpty() ? QModelIndex() : selection.first();
}

void BookIndexEditor::swap(QModelIndex fromIndex, QModelIndex toIndex)
{
    if(fromIndex.parent() != toIndex.parent())
        return;

    QModelIndex parent = fromIndex.parent();

    QStandardItem *parentItem = 0;
    if(parent.isValid())
        parentItem = m_model->itemFromIndex(parent);
    else
        parentItem = m_model->invisibleRootItem();

    if(!parentItem)
        return;

    QStandardItem *fromItem = m_model->itemFromIndex(fromIndex);

    if(!fromItem)
        return;

    parentItem->takeRow(fromIndex.row());
    parentItem->insertRow(toIndex.row(), fromItem);
}

QModelIndex BookIndexEditor::changeParent(QModelIndex child, QModelIndex newParent, int row)
{
    QModelIndex currentParent = child.parent();

    QStandardItem *currentParentItem = 0;
    if(currentParent.isValid())
        currentParentItem = m_model->itemFromIndex(currentParent);
    else
        currentParentItem = m_model->invisibleRootItem();

    QStandardItem *childItem = currentParentItem->takeChild(child.row(), child.column());
    currentParentItem->takeRow(child.row());

    QStandardItem *newParentItem = 0;
    if(newParent.isValid())
        newParentItem = m_model->itemFromIndex(newParent);
    else
        newParentItem = m_model->invisibleRootItem();

    if(!newParentItem)
        return QModelIndex();

    if(row == -1)
        row = newParentItem->rowCount();

    newParentItem->insertRow(row, childItem);

    QModelIndex insertedIndex = newParent.child(row, 0);
    if(!insertedIndex.isValid())
        insertedIndex = m_model->index(row, 0);

    return insertedIndex;
}

void BookIndexEditor::selectIndex(QModelIndex index)
{
    if(index.isValid()) {
        ui->treeView->scrollTo(index, QAbstractItemView::EnsureVisible);
        ui->treeView->selectionModel()->setCurrentIndex(index,
                                                        QItemSelectionModel::ClearAndSelect);
    }
}

bool BookIndexEditor::save(QString path)
{
    qDebug() << "Save title:" << path;
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning("Can't open file %s for writing", qPrintable(path));
        return false;
    }

    QXmlStreamWriter writer(&file);
    writer.setCodec("utf-8");
    writer.setAutoFormatting(true);

    saveModel(&writer);

    return true;
}

void BookIndexEditor::addTitle()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("عنوان جديد"),
                                         tr("العنوان الجديد:"), QLineEdit::Normal,
                                         m_editView->m_webView->selectedText(), &ok);
    if(ok && !text.isEmpty()) {
        QModelIndex index = selectedIndex();
        QStandardItem *title = new QStandardItem(text);
        title->setData(m_editView->m_currentPage->pageID, ItemRole::idRole);

        QStandardItem *parentItem = 0;
        if(index.isValid() && index.parent().isValid())
            parentItem = m_model->itemFromIndex(index.parent());
        else
            parentItem = m_model->invisibleRootItem();

        int row = index.isValid() ? index.row()+1 : parentItem->rowCount();

        parentItem->insertRow(row, title);
        selectIndex(m_model->index(row, 0, index.parent()));
    }
}

void BookIndexEditor::removeTitle()
{
    QModelIndex index = selectedIndex();
    if(!index.isValid())
        return;

    m_model->removeRow(index.row(), index.parent());
    ui->treeView->clearSelection();
}

void BookIndexEditor::moveUp()
{
    QModelIndex index = selectedIndex();
    if(!index.isValid())
        return;

    QModelIndex toIndex = index.sibling(index.row()-1, index.column());
    if(!toIndex.isValid())
        return;

    ui->treeView->collapse(index);
    ui->treeView->collapse(toIndex);
    ui->treeView->scrollTo(toIndex, QAbstractItemView::EnsureVisible);

    swap(index, toIndex);
    selectIndex(toIndex);
}

void BookIndexEditor::moveDown()
{
    QModelIndex index = selectedIndex();
    if(!index.isValid())
        return;

    QModelIndex toIndex = index.sibling(index.row()+1, index.column());
    if(!toIndex.isValid())
        return;

    ui->treeView->collapse(index);
    ui->treeView->collapse(toIndex);
    ui->treeView->scrollTo(toIndex, QAbstractItemView::EnsureVisible);

    swap(index, toIndex);
    selectIndex(toIndex);
}

void BookIndexEditor::moveRight()
{
    QModelIndex index = selectedIndex();
    QModelIndex parent = index.parent();
    if(!index.isValid() || !parent.isValid())
        return;

    QModelIndex newParent = parent.parent();

    QModelIndex changedIndex = changeParent(index, newParent, parent.row()+1);
    selectIndex(changedIndex);
}

void BookIndexEditor::moveLeft()
{
    QModelIndex index = selectedIndex();
    if(!index.isValid())
        return;

    QModelIndex parent = index.sibling(index.row()-1, index.column());

    QModelIndex changedIndex = changeParent(index, parent);
    selectIndex(changedIndex);
}

void BookIndexEditor::linkTitle()
{
    QModelIndex index = selectedIndex();
    if(!index.isValid())
        return;

    m_editView->m_currentPage->titleID = m_editView->m_currentPage->pageID;
    ui->treeView->model()->setData(index, m_editView->m_currentPage->pageID, ItemRole::idRole);
}

void BookIndexEditor::updateActions()
{
    QModelIndex index = selectedIndex();
    QModelIndex prevIndex = index.sibling(index.row()+1, index.column());
    QModelIndex nextIndex = index.sibling(index.row()-1, index.column());

    ui->toolMoveUp->setEnabled(nextIndex.isValid());
    ui->toolMoveDown->setEnabled(prevIndex.isValid());
    ui->toolRemoveTitle->setEnabled(index.isValid());
    ui->toolLinkTitle->setEnabled(index.isValid());
    ui->toolMoveRight->setEnabled(index.parent().isValid());
    ui->toolMoveLeft->setEnabled(index.sibling(index.row()-1, 0).isValid());
}
