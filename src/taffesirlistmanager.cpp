#include "taffesirlistmanager.h"
#include "mainwindow.h"
#include "libraryinfo.h"
#include "modelenums.h"
#include "xmlutils.h"
#include "utils.h"
#include "shamelaimportinfo.h"

#include <qfile.h>
#include <qxmlstream.h>
#include <qdir.h>
#include <qdom.h>

static TaffesirListManager *m_instance = 0;

TaffesirListManager::TaffesirListManager(QObject *parent) : XmlManager(parent)
{
    QDir dataDir(MW->libraryInfo()->dataDir());
    m_dom.setFilePath(dataDir.filePath("taffesirlist.xml"));
    m_model = 0;

    ml_set_instance(m_instance, this);
}

TaffesirListManager::~TaffesirListManager()
{
    clear();

    m_instance = 0;
}

void TaffesirListManager::loadModels()
{
    m_dom.load();

    taffesirListModel();
    emit ModelsReady();
}

void TaffesirListManager::clear()
{
    ml_delete_check(m_model);
}

QStandardItemModel *TaffesirListManager::taffesirListModel()
{
    if(!m_model)
        m_model = getModel(false);

    return m_model;
}

QStandardItemModel *TaffesirListManager::allTaffesirModel()
{
    return getModel(true);
}

QStandardItemModel *TaffesirListManager::getModel(bool allTaffasir)
{
    QStandardItemModel *model = new QStandardItemModel();

    model->setHorizontalHeaderLabels(QStringList() << tr("التفسير")
                                     << tr("عرض عند تصفح القرآن الكريم"));

    QDomElement e = m_dom.rootElement().firstChildElement();
    while(!e.isNull()) {
        Qt::CheckState checkStat = e.attribute("show") == "false" ? Qt::Unchecked : Qt::Checked;
        if(allTaffasir || checkStat) {
            QStandardItem *item = new QStandardItem();
            item->setText(e.firstChildElement("title").text());
            item->setToolTip(item->text());
            item->setData(e.attribute("bookID").toInt(), ItemRole::idRole);
            item->setIcon(QIcon(":/images/book.png"));

            if(!allTaffasir)
                item->setText(Utils::String::abbreviate(item->text(), 35));

            QStandardItem *item2 = new QStandardItem();
            item2->setCheckable(true);
            item2->setCheckState(checkStat);

            model->appendRow(QList<QStandardItem*>() << item << item2);
        }

        e = e.nextSiblingElement();
    }

    return model;
}

void TaffesirListManager::addTafessir(int bookID, QString taffesirName)
{
    QMutexLocker locker(&m_mutex);

    QDomElement taffesirElement = m_dom.domDocument().createElement("taffesir");
    taffesirElement.setAttribute("bookID", bookID);
    taffesirElement.setAttribute("show", "true");

    QDomElement titleElement = m_dom.domDocument().createElement("title");
    titleElement.appendChild(m_dom.domDocument().createTextNode(taffesirName));

    taffesirElement.appendChild(titleElement);
    m_dom.rootElement().appendChild(taffesirElement);

    m_dom.setNeedSave(true);
}

void TaffesirListManager::saveModel(QXmlStreamWriter &writer, QStandardItemModel *model)
{
    writer.writeStartDocument();
    writer.writeStartElement("taffesir-list");

    for(int i=0; i<model->rowCount(); i++) {
        writer.writeStartElement("taffesir");
        writer.writeAttribute("bookID", model->item(i, 0)->data(ItemRole::idRole).toString());
        writer.writeAttribute("show", model->item(i, 1)->checkState() == Qt::Unchecked ? "false" : "true");

        writer.writeTextElement("title", model->item(i, 0)->text());

        writer.writeEndElement();
    }

    writer.writeEndElement();
    writer.writeEndDocument();
}
