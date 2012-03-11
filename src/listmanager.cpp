#include "listmanager.h"
#include "xmlutils.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qdebug.h>
#include <qxmlstream.h>

ListManager::ListManager(QObject *parent) :
    QObject(parent),
    m_domLoaded(false),
    m_saveDom(false)
{
}

ListManager::~ListManager()
{
}

void ListManager::setFilePath(QString path)
{
    m_dom.setFilePath(path);
}

void ListManager::clear()
{
}

void ListManager::save(QStandardItemModel *model)
{
    if(m_dom.needSave()) {
        qCritical("ListManager: The XML need to be save before saving the model");
        m_dom.setNeedSave(false);
    }

    QFile file(m_dom.filePath());

    if(!file.open(QIODevice::WriteOnly)) {
        qCritical() << "Can't open file" << m_dom.filePath();
        return;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);

    saveModel(writer, model);

    file.close();
    reloadModels();
}

void ListManager::saveModel(QXmlStreamWriter &writer, QStandardItemModel *model)
{
    Q_UNUSED(writer);
    Q_UNUSED(model);
}

void ListManager::reloadModels()
{
    m_dom.reload();
    clear();
    loadModels();
}
