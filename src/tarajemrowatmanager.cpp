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

TarajemRowatManager::TarajemRowatManager(QObject *parent) : XmlManager(parent)
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
    clear();
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
    qDeleteAll(m_info);
    m_info.clear();
    m_fullInfo.clear();

    m_elementHash.clear();
    m_newElements.clear();
    m_removedRowat.clear();
}

void TarajemRowatManager::reloadModels()
{
    clear();
    loadModels();
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
    m_zip.unzip();
    ML_ASSERT_RET2(m_zip.zipStat() == ZipHelper::UnZipped, "beginUpdate: zip is not in UnZipped stat", false);

    m_domHelper = m_zip.getDomHelper("rowat.xml", "rowat");
    ML_ASSERT_RET2(!m_domHelper.isNull(), "beginUpdate: DomHelper is null", false);

    m_elementHash.clear();

    for(int i=0; i<m_newElements.size();i++) {
        if(m_domHelper->rootElement().appendChild(m_newElements[i]).isNull())
            qWarning("beginUpdate: Error on appendChild at index %d", i);
    }

    QDomElement e = m_domHelper->rootElement().firstChildElement();
    while(!e.isNull()) {
        int rawiID = e.attribute("id").toInt();
        if(m_removedRowat.contains(rawiID)) {
            qWarning("beginUpdate: Remove rawi %d", rawiID);

            if(m_domHelper->rootElement().removeChild(e).isNull())
                qWarning("beginUpdate: Error on removeChild for rawi %d", rawiID);
        } else {
            m_elementHash.insert(rawiID, e);
        }

        e = e.nextSiblingElement();
    }

    return true;
}

void TarajemRowatManager::endUpdate()
{
    m_elementHash.clear();
    m_newElements.clear();
    m_removedRowat.clear();

    m_domHelper.clear();
    m_zip.save();

    reloadModels();
}

void TarajemRowatManager::updateRawi(RawiInfo *rawi)
{
    QDomElement e = m_elementHash.value(rawi->id);
    ML_ASSERT2(!e.isNull(), "updateRawi: Rawi Dom Element is null");

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

    QString infoFileName = QString("data/r%1.xml").arg(rawi->id);

    XmlDomHelperPtr infoDom = m_zip.getDomHelper(infoFileName, "rawi-info");
        if(!infoDom.isNull()) {
            infoDom->setElementText(infoDom->rootElement(), "sheok", rawi->sheok, true);
            infoDom->setElementText(infoDom->rootElement(), "talamid", rawi->talamid, true);
            infoDom->setElementText(infoDom->rootElement(), "tarejama", rawi->tarejama, true);

            infoDom->setNeedSave(true);
        } else {
            qDebug("updateRawi: infoDom is null");
        }

    m_domHelper->setNeedSave(true);
}

int TarajemRowatManager::addRawi(RawiInfo *rawi)
{
    QMutexLocker locker(&m_mutex);

    if(!rawi->id)
        rawi->id = getNewRawiID();

    ZipOpener opener(&m_zip);

    XmlDomHelperPtr dom = m_zip.getDomHelper("rowat.xml");
    if(dom.isNull()) {
        qCritical("addRawi: Dom is null");
        return 0;
    }

    QDomElement rawiElement = dom->domDocument().createElement("rawi");
    rawiElement.setAttribute("id", rawi->id);

    QDomElement nameEelement = dom->domDocument().createElement("name");
    QDomElement laqabEelement = dom->domDocument().createElement("laqab");
    QDomElement tabaqaEelement = dom->domDocument().createElement("tabaqa");
    QDomElement rowatEelement = dom->domDocument().createElement("rowat");

    nameEelement.appendChild(dom->domDocument().createTextNode(rawi->name));
    laqabEelement.appendChild(dom->domDocument().createTextNode(rawi->laqab));
    tabaqaEelement.appendChild(dom->domDocument().createTextNode(rawi->tabaqa));
    rowatEelement.appendChild(dom->domDocument().createTextNode(rawi->rowat));

    QDomElement hafedEelement = dom->domDocument().createElement("hafed");
    QDomElement zahabiEelement = dom->domDocument().createElement("zahabi");

    hafedEelement.appendChild(dom->domDocument().createTextNode(rawi->rotba_hafed));
    zahabiEelement.appendChild(dom->domDocument().createTextNode(rawi->rotba_zahabi));

    QDomElement rotbaElement = dom->domDocument().createElement("rotba");
    rotbaElement.appendChild(hafedEelement);
    rotbaElement.appendChild(zahabiEelement);

    rawiElement.appendChild(nameEelement);
    rawiElement.appendChild(laqabEelement);
    rawiElement.appendChild(tabaqaEelement);
    rawiElement.appendChild(rowatEelement);
    rawiElement.appendChild(rotbaElement);

    if(!rawi->unknowBirth()) {
        QDomElement birthElement = dom->domDocument().createElement("birth");
        birthElement.setAttribute("year", rawi->birthYear);
        birthElement.appendChild(dom->domDocument().createTextNode(rawi->birthStr));

        rawiElement.appendChild(birthElement);
    }

    if(!rawi->unknowDeath()) {
        QDomElement deathElement = dom->domDocument().createElement("death");
        deathElement.setAttribute("year", rawi->deathYear);
        deathElement.appendChild(dom->domDocument().createTextNode(rawi->deathStr));

        rawiElement.appendChild(deathElement);
    }

    m_info[rawi->id] = rawi;
    m_fullInfo[rawi->id] = rawi;

    m_newElements.append(rawiElement);

    return rawi->id;
}

void TarajemRowatManager::removeRawi(int rawiID)
{
    ML_ASSERT2(m_info.contains(rawiID), "removeRawi: no rawi with id" << rawiID);

    m_removedRowat.append(rawiID);
    RawiInfo *rawi = m_info.take(rawiID);
    m_fullInfo.remove(rawiID);

    ML_ASSERT2(rawi->id == rawiID, "removeRawi: Can't remove rawi" << rawiID << "from saved rowat info");

    delete rawi;
}

int TarajemRowatManager::getNewRawiID()
{
    int rawiID = 0;
    do {
        rawiID = Utils::randInt(11111, 99999);
    } while(m_info.contains(rawiID));

    return rawiID;
}
