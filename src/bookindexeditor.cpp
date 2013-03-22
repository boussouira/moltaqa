#include "bookindexeditor.h"
#include "ui_bookindexeditor.h"
#include "modelenums.h"
#include "modelutils.h"
#include "editwebview.h"
#include "richbookreader.h"
#include "bookeditorview.h"
#include "utils.h"
#include "mainwindow.h"
#include "libraryinfo.h"

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

    m_treeManager = new TreeViewEditor(this);

    m_treeManager->setMoveUpButton(ui->toolMoveUp);
    m_treeManager->setMoveDownButton(ui->toolMoveDown);
    m_treeManager->setMoveLeftButton(ui->toolMoveLeft);
    m_treeManager->setMoveRightButton(ui->toolMoveRight);
    m_treeManager->setRemovButton(ui->toolRemoveTitle);

    m_indexEdited = false;

    connect(ui->toolAddTitle, SIGNAL(clicked()), SLOT(addTitle()));
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

    m_treeManager->setTreeView(ui->treeView);
    m_treeManager->setModel(m_model);
    m_treeManager->setup();

    connect(ui->treeView->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(updateActions()));

    connect(m_treeManager, SIGNAL(modelDataChanged()), SIGNAL(indexDataChanged()));
    connect(m_treeManager, SIGNAL(modelDataChanged()), SLOT(indexEditedSlot()));

    m_indexEdited = false;

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
    writer->writeAttribute("tagID", item->data(ItemRole::titleIdRole).toString());
    writer->writeTextElement("text", item->text());

    if(item->hasChildren()) {
        for(int i=0; i<item->rowCount(); i++) {
            QStandardItem *child = item->child(i);
            writeItem(child, writer);
        }
    }

    writer->writeEndElement();
}

bool BookIndexEditor::save(ZipHelper *zipHelper)
{
    if(!m_indexEdited) {
#ifdef DEV_BUILD
        qWarning() << "BookIndexEditor::save No need to save titles model";
#endif

        return true;
    }

    QString path = Utils::Rand::fileName(MW->libraryInfo()->tempDir(),
                                            true, "book_index_", "xml");

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

    file.close();

    zipHelper->replaceFromFile("titles.xml",
                               path,
                               ZipHelper::PrependFile);

    file.remove();

    m_treeManager->setDataChanged(false);
    m_indexEdited = false;

    return true;
}

bool BookIndexEditor::indexEdited() const
{
    return m_indexEdited;
}

void BookIndexEditor::setIndexEdited(bool edited)
{
#ifdef DEV_BUILD
    if(m_indexEdited && !edited)
        qDebug("BookIndexEditor::setIndexEdited model need to be saved");
#endif

    m_indexEdited = edited;
}

void BookIndexEditor::addTitle()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("عنوان جديد"),
                                         tr("العنوان الجديد:"), QLineEdit::Normal,
                                         m_editView->m_webView->selectedText().simplified(), &ok);
    if(ok && text.size()) {
        QModelIndex index = Utils::Model::selectedIndex(ui->treeView);
        QStandardItem *title = new QStandardItem(text);

        int pageID = (m_editView->m_currentPage ? m_editView->m_currentPage->pageID : 1);
        QString tagID = Utils::Rand::string(6, false);
        title->setData(pageID, ItemRole::idRole);
        title->setData(tagID, ItemRole::titleIdRole);

        QStandardItem *parentItem = Utils::Model::itemFromIndex(m_model, index.parent());
        int row = index.isValid() ? index.row()+1 : parentItem->rowCount();

        parentItem->insertRow(row, title);
        Utils::Model::selectIndex(ui->treeView, m_model->index(row, 0, index.parent()));

        int count = 1;
        QModelIndex parent = index.parent();
        while (parent.isValid()) {
            ++count;
            parent = parent.parent();
        }

        m_editView->m_webView->makeSelectTextTitle(text, qBound(1, count, 6), tagID);

        emit indexDataChanged();
    }
}

void BookIndexEditor::linkTitle()
{
    ml_return_on_fail(m_editView->m_currentPage);

    QModelIndex index = Utils::Model::selectedIndex(ui->treeView);
    ml_return_on_fail(index.isValid());

    m_editView->m_currentPage->titleID = m_editView->m_currentPage->pageID;
    ui->treeView->model()->setData(index, m_editView->m_currentPage->pageID, ItemRole::idRole);

    QDomElement pageElement = m_editView->m_bookReader->pagesDom().findElement("id",
                                                                               m_editView->m_currentPage->pageID);
    if(!pageElement.isNull())
        pageElement.removeAttribute("tid");

    emit indexDataChanged();
}

void BookIndexEditor::updateActions()
{
    QModelIndex index = Utils::Model::selectedIndex(ui->treeView);

    ui->toolLinkTitle->setEnabled(index.isValid());
}

void BookIndexEditor::openPage(QModelIndex index)
{
    m_editView->m_bookReader->goToPage(index.data(ItemRole::idRole).toInt());
}

void BookIndexEditor::indexEditedSlot()
{
    m_indexEdited = true;
}
