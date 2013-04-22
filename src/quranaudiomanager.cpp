#include "quranaudiomanager.h"
#include "libraryinfo.h"
#include "librarymanager.h"
#include "mainwindow.h"

#include <qdir.h>

QuranAudioManager::QuranAudioManager(QObject *parent) :
    XmlManager(parent),
    m_model(0)
{
    QDir dataDir(MW->libraryInfo()->dataDir());
    m_dom.setFilePath(dataDir.filePath("reciter.xml"));
}

QuranAudioManager::~QuranAudioManager()
{
    clear();
}

void QuranAudioManager::loadModels()
{
    m_dom.load();
}

void QuranAudioManager::clear()
{
}

QStandardItemModel *QuranAudioManager::recitersModel()
{
    if(!m_model)
        m_model = getModel();

    return m_model;
}

void QuranAudioManager::saveModel(QXmlStreamWriter &writer, QStandardItemModel *model)
{
    Q_UNUSED(writer);
    Q_UNUSED(model);
}

QStandardItemModel *QuranAudioManager::getModel()
{
    QStandardItemModel *model = new QStandardItemModel();

    model->setHorizontalHeaderLabels(QStringList() << tr("المقرئ"));

    QDomElement e = m_dom.rootElement().firstChildElement();
    while(!e.isNull()) {
        QStandardItem *item = new QStandardItem();
        item->setText(e.firstChildElement("name").text());
        item->setToolTip(item->text());
        item->setData(e.firstChildElement("url").text(), Qt::UserRole);

        model->appendRow(item);

        e = e.nextSiblingElement();
    }

    return model;
}
