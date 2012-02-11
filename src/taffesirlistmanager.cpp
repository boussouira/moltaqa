#include "taffesirlistmanager.h"
#include "mainwindow.h"
#include "libraryinfo.h"
#include "modelenums.h"
#include "xmlutils.h"
#include <qfile.h>
#include <qxmlstream.h>
#include <qdir.h>
#include <qdom.h>
#include "shamelaimportinfo.h"

TaffesirListManager::TaffesirListManager(QObject *parent) : ListManager(parent)
{
    QDir dataDir(MW->libraryInfo()->dataDir());
    m_filePath = dataDir.filePath("taffesirlist.xml");
    m_model = 0;

    reloadModel();
}

TaffesirListManager::~TaffesirListManager()
{
    clear();
}

void TaffesirListManager::reloadModel()
{
    reloadXmlDom();
    clear();

    m_model = getModel(false);
}

void TaffesirListManager::clear()
{
    if(m_model)
        delete m_model;
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

    QDomElement e = m_rootElement.firstChildElement();
    while(!e.isNull()) {
        Qt::CheckState checkStat = e.attribute("show") == "false" ? Qt::Unchecked : Qt::Checked;
        if(allTaffasir || checkStat) {
            QStandardItem *item = new QStandardItem();
            item->setText(e.firstChildElement("title").text());
            item->setData(e.attribute("bookID").toInt(), ItemRole::idRole);

            QStandardItem *item2 = new QStandardItem();
            item2->setCheckable(true);
            item2->setCheckState(checkStat);

            model->appendRow(QList<QStandardItem*>() << item << item2);
        }

        e = e.nextSiblingElement();
    }

    if(!allTaffasir)
        emit taffesirModelReady();

    return model;
}

void TaffesirListManager::addTafessir(int bookID, QString taffesirName)
{
    QMutexLocker locker(&m_mutex);

    QDomElement taffesirElement = m_doc.createElement("taffesir");
    taffesirElement.setAttribute("bookID", bookID);
    taffesirElement.setAttribute("show", "true");

    QDomElement titleElement = m_doc.createElement("title");
    titleElement.appendChild(m_doc.createTextNode(taffesirName));

    taffesirElement.appendChild(titleElement);
    m_rootElement.appendChild(taffesirElement);

    m_saveDom = true;
}

void TaffesirListManager::save(QStandardItemModel *taffesirModel)
{
    if(m_saveDom)
        qCritical("TaffesirListManager: The XML need to be save before saving the model");

    QFile taffesirFile(m_filePath);

    if(!taffesirFile.open(QIODevice::WriteOnly)) {
        qCritical("Can't open taffesirlist.xml");
        return;
    }

    QXmlStreamWriter writer(&taffesirFile);
    writer.setAutoFormatting(true);

    writer.writeStartDocument();
    writer.writeStartElement("taffesir-list");

    for(int i=0; i<taffesirModel->rowCount(); i++) {
        writer.writeStartElement("taffesir");
        writer.writeAttribute("bookID", taffesirModel->item(i, 0)->data(ItemRole::idRole).toString());
        writer.writeAttribute("show", taffesirModel->item(i, 1)->checkState() == Qt::Unchecked ? "false" : "true");

        writer.writeTextElement("title", taffesirModel->item(i, 0)->text());

        writer.writeEndElement();
    }

    writer.writeEndElement();
    writer.writeEndDocument();

    taffesirFile.close();

    reloadModel();
}
