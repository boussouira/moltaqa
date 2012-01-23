#include "bookindexeditor.h"
#include "ui_bookindexeditor.h"
#include "bookindexmodel.h"
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

void BookIndexEditor::setModel(BookIndexModel *model)
{
    m_model = model;
    ui->treeView->setModel(model);
}

void BookIndexEditor::saveModel(QXmlStreamWriter *writer)
{
    Q_CHECK_PTR(m_model);

    BookIndexNode *root = m_model->nodeFromIndex(QModelIndex());

    if(root) {
        writer->writeStartDocument();
        writer->writeStartElement("titles");

        if(!root->childs.isEmpty()) {
            foreach(BookIndexNode *child, root->childs) {
                writeNode(child, writer);
            }
        }

        writer->writeEndElement();
        writer->writeEndDocument();
    }
}

void BookIndexEditor::writeNode(BookIndexNode *node, QXmlStreamWriter *writer)
{
    writer->writeStartElement("item");
    writer->writeAttribute("pageID", QString::number(node->id));
    writer->writeAttribute("text", node->title);

    if(!node->childs.isEmpty()) {
        foreach(BookIndexNode *child, node->childs) {
            writeNode(child, writer);
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
