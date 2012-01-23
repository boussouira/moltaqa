#include "bookindexeditor.h"
#include "ui_bookindexeditor.h"
#include "modelenums.h"
#include <qstandarditemmodel.h>
#include <qfile.h>

BookIndexEditor::BookIndexEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BookIndexEditor)
{
    ui->setupUi(this);
}

BookIndexEditor::~BookIndexEditor()
{
    delete ui;
}

void BookIndexEditor::setModel(QStandardItemModel *model)
{
    m_model = model;
    ui->treeView->setModel(model);
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
            QStandardItem *child = m_model->item(i);
            writeItem(child, writer);
        }
    }

    writer->writeEndElement();
}

bool BookIndexEditor::save(QString path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning("Can't open file %s for writing", qPrintable(path));
        return false;
    }

    QXmlStreamWriter writer(&file);
    writer.setCodec("utf-8");
    writer.setAutoFormatting(true);

    saveModel(&writer);

    return true;
}
