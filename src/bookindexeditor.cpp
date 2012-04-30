#include "bookindexeditor.h"
#include "ui_bookindexeditor.h"
#include "modelenums.h"
#include "modelutils.h"
#include "editwebview.h"
#include "richbookreader.h"
#include "bookeditorview.h"
#include "utils.h"

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
    connect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)), SLOT(openPage(QModelIndex)));
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
    ml_return_on_fail2(model, "BookIndexEditor::setModel model is null");

    m_model = model;
    ui->treeView->setModel(model);

    connect(ui->treeView->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(updateActions()));

    connect(m_model, SIGNAL(layoutChanged()), SIGNAL(indexEdited()));

    updateActions();
}

void BookIndexEditor::saveModel(QXmlStreamWriter *writer)
{
    ml_return_on_fail2(m_model, "BookIndexEditor::setModel model is null");

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
    writer->writeStartElement("title");
    writer->writeAttribute("pageID", item->data(ItemRole::idRole).toString());
    writer->writeTextElement("text", item->text());

    if(item->hasChildren()) {
        for(int i=0; i<item->rowCount(); i++) {
            QStandardItem *child = item->child(i);
            writeItem(child, writer);
        }
    }

    writer->writeEndElement();
}

bool BookIndexEditor::save(QString path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "BookIndexEditor::save Can't open file"
                   << path << "for writing" << file.errorString();
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
    if(ok && text.size()) {
        QModelIndex index = Utils::Model::selectedIndex(ui->treeView);
        QStandardItem *title = new QStandardItem(text);
        title->setData(m_editView->m_currentPage->pageID, ItemRole::idRole);

        QStandardItem *parentItem = Utils::Model::itemFromIndex(m_model, index.parent());
        int row = index.isValid() ? index.row()+1 : parentItem->rowCount();

        parentItem->insertRow(row, title);
        Utils::Model::selectIndex(ui->treeView, m_model->index(row, 0, index.parent()));
    }
}

void BookIndexEditor::removeTitle()
{
    QModelIndex index = Utils::Model::selectedIndex(ui->treeView);
    ml_return_on_fail(index.isValid());

    m_model->removeRow(index.row(), index.parent());
    ui->treeView->clearSelection();
}

void BookIndexEditor::moveUp()
{
    Utils::Model::moveUp(m_model, ui->treeView);
}

void BookIndexEditor::moveDown()
{
    Utils::Model::moveDown(m_model, ui->treeView);
}

void BookIndexEditor::moveRight()
{
    Utils::Model::moveRight(m_model, ui->treeView);
}

void BookIndexEditor::moveLeft()
{
    Utils::Model::moveLeft(m_model, ui->treeView);
}

void BookIndexEditor::linkTitle()
{
    QModelIndex index = Utils::Model::selectedIndex(ui->treeView);
    ml_return_on_fail(index.isValid());

    m_editView->m_currentPage->titleID = m_editView->m_currentPage->pageID;
    ui->treeView->model()->setData(index, m_editView->m_currentPage->pageID, ItemRole::idRole);
}

void BookIndexEditor::updateActions()
{
    QModelIndex index = Utils::Model::selectedIndex(ui->treeView);
    QModelIndex prevIndex = index.sibling(index.row()+1, index.column());
    QModelIndex nextIndex = index.sibling(index.row()-1, index.column());

    ui->toolMoveUp->setEnabled(nextIndex.isValid());
    ui->toolMoveDown->setEnabled(prevIndex.isValid());
    ui->toolRemoveTitle->setEnabled(index.isValid());
    ui->toolLinkTitle->setEnabled(index.isValid());
    ui->toolMoveRight->setEnabled(index.parent().isValid());
    ui->toolMoveLeft->setEnabled(index.sibling(index.row()-1, 0).isValid());
}

void BookIndexEditor::openPage(QModelIndex index)
{
    m_editView->m_bookReader->goToPage(index.data(ItemRole::idRole).toInt());
}
