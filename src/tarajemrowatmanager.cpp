#include "tarajemrowatmanager.h"
#include "mainwindow.h"
#include "librarymanager.h"
#include "libraryinfo.h"
#include "modelenums.h"
#include "utils.h"
#include "xmlutils.h"
#include "authorsmanager.h"
#include "zipopener.h"

#include <qdir.h>
#include <qstandarditemmodel.h>

TarajemRowatManager *m_instance = 0;

TarajemRowatManager::TarajemRowatManager(QObject *parent) : ListManager(parent)
{
    QDir dataDir(MW->libraryInfo()->dataDir());
    m_path = dataDir.filePath("rowat.zip");
    m_zip.setPath(m_path);

    m_instance = this;

    loadModels();
}

TarajemRowatManager::~TarajemRowatManager()
{
    m_instance = 0;

    qDeleteAll(m_info);
    m_info.clear();
}

TarajemRowatManager *TarajemRowatManager::instance()
{
    return m_instance;
}

void TarajemRowatManager::loadModels()
{
    ZipOpener opener(&m_zip);

    XmlDomHelperPtr dom = m_zip.getDomHelper("rowat.xml");
    if(!dom.isNull()) {
        QDomElement e = dom->rootElement().firstChildElement();
        while(!e.isNull()) {
            RawiInfo *rawi = new RawiInfo();
            rawi->id = e.attribute("id").toInt();
            rawi->name = e.firstChildElement("name").text();

            m_info[rawi->id] = rawi;

            e = e.nextSiblingElement();
        }
    }
}

void TarajemRowatManager::clear()
{
}

QStandardItemModel *TarajemRowatManager::getRowatModel()
{
    QStandardItemModel *model = new QStandardItemModel();

    model->setHorizontalHeaderLabels(QStringList() << tr("الرواة"));

    foreach(RawiInfo *rawi, m_info.values()) {
        QStandardItem *item = new QStandardItem();
        item->setText(Utils::abbreviate(rawi->name, 100));
        item->setToolTip(rawi->name);
        item->setData(rawi->id, ItemRole::authorIdRole);

        model->appendRow(item);
    }

    return model;
}

RawiInfo *TarajemRowatManager::getRawiInfo(int rawiID)
{
    RawiInfo *rawi = m_fullInfo.value(rawiID);
    if(rawi)
        return rawi;

    ZipOpener opener(&m_zip);

    XmlDomHelperPtr dom = m_zip.getDomHelper("rowat.xml");
    if(!dom.isNull()) {
        QString rawiIDStr = QString::number(rawiID);
        QDomElement e = dom->rootElement().firstChildElement();
        while(!e.isNull()) {
            if(rawiIDStr == e.attribute("id")) {
                rawi = m_info.value(rawiID);
                if(!rawi)
                    rawi = new RawiInfo();

                rawi->name = e.firstChildElement("name").text();
                rawi->tabaqa = e.firstChildElement("tabaqa").text();
                rawi->laqab = e.firstChildElement("laqab").text();
                rawi->rowat = e.firstChildElement("rowat").text();
                rawi->rotba_hafed = e.firstChildElement("rotba").firstChildElement("hafed").text();
                rawi->rotba_zahabi = e.firstChildElement("rotba").firstChildElement("zahabi").text();

                QDomElement birthElement = e.firstChildElement("birth");
                if(!birthElement.isNull()) {
                    if(birthElement.hasAttribute("year"))
                        rawi->birthYear = birthElement.attribute("year").toInt();

                    QString text = birthElement.text().simplified();
                    if(text.isEmpty() && !rawi->unknowBirth())
                        text = Utils::hijriYear(rawi->birthYear);

                    rawi->birthStr = text;
                }

                QDomElement deathElement = e.firstChildElement("death");
                if(!deathElement.isNull()) {
                    if(deathElement.hasAttribute("year"))
                        rawi->deathYear = deathElement.attribute("year").toInt();

                    QString text = deathElement.text().simplified();
                    if(text.isEmpty() && !rawi->unknowDeath())
                        text = Utils::hijriYear(rawi->deathYear);

                    rawi->deathStr = text;
                }

                XmlDomHelperPtr infoDom = m_zip.getDomHelper(QString("data/r%1.xml").arg(rawiID));
                rawi->sheok = infoDom->rootElement().firstChildElement("sheok").text();
                rawi->talamid = infoDom->rootElement().firstChildElement("talamid").text();
                rawi->tarejama = infoDom->rootElement().firstChildElement("tarejama").text();

                m_info[rawi->id] = rawi;
                m_fullInfo[rawi->id] = rawi;

                break;
            }

            e = e.nextSiblingElement();
        }
    }

    return rawi;
}

bool TarajemRowatManager::beginUpdate()
{
    ML_RETURN_FALSE(m_zip.unzip().isEmpty());

    m_domHelper = m_zip.getDomHelper("rowat.xml");
    ML_RETURN_FALSE(m_domHelper.isNull());

    m_elementHash.clear();
    QDomElement e = m_domHelper->rootElement().firstChildElement();
    while(!e.isNull()) {
        m_elementHash.insert(e.attribute("id").toInt(), e);

        e = e.nextSiblingElement();
    }

    return true;
}

void TarajemRowatManager::endUpdate()
{
    m_elementHash.clear();
    m_domHelper.clear();
    m_zip.save();
}

void TarajemRowatManager::updateRawi(RawiInfo *rawi)
{
    QDomElement e = m_elementHash.value(rawi->id);
    ML_ASSERT(!e.isNull());

    e.setAttribute("id", rawi->id);

    m_domHelper->setElementText(e, "name", rawi->name);
    m_domHelper->setElementText(e, "laqab", rawi->laqab);
    m_domHelper->setElementText(e, "tabaqa", rawi->tabaqa);
    m_domHelper->setElementText(e, "rowat", rawi->rowat);

    QDomElement birthElement = Utils::findChildElement(e, m_domHelper->domDocument(), "birth");
    if(rawi->unknowBirth()) {
        e.removeChild(birthElement);
    } else {
        birthElement.setAttribute("year", rawi->birthYear);
        Utils::findChildText(birthElement, m_domHelper->domDocument()).setNodeValue(rawi->birthStr);
    }

    QDomElement deathElement = Utils::findChildElement(e, m_domHelper->domDocument(), "death");
    if(rawi->unknowDeath()) {
        e.removeChild(deathElement);
    } else {
        deathElement.setAttribute("year", rawi->deathYear);
        Utils::findChildText(deathElement, m_domHelper->domDocument()).setNodeValue(rawi->deathStr);
    }

    QDomElement rotbaElement = Utils::findChildElement(e, m_domHelper->domDocument(), "rotba");
    m_domHelper->setElementText(rotbaElement, "hafed", rawi->rotba_hafed);
    m_domHelper->setElementText(rotbaElement, "zahabi", rawi->rotba_zahabi);

    XmlDomHelperPtr infoDom = m_zip.getDomHelper(QString("data/r%1.xml").arg(rawi->id));
    if(!infoDom.isNull()) {
        infoDom->setElementText(infoDom->rootElement(), "sheok", rawi->sheok);
        infoDom->setElementText(infoDom->rootElement(), "talamid", rawi->talamid);
        infoDom->setElementText(infoDom->rootElement(), "tarejama", rawi->tarejama);

        infoDom->setNeedSave(true);
    } else {
        qDebug("updateRawi: infoDom is null");
    }

    m_domHelper->setNeedSave(true);
}
