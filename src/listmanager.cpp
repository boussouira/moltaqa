#include "listmanager.h"
#include "xmlutils.h"
#include "utils.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qdebug.h>
#include <qxmlstream.h>

ListManager::ListManager(QObject *parent) :
    QObject(parent)
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

    ML_ASSERT2(file.open(QIODevice::WriteOnly), "ListManager::save open file error" << file.errorString());

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
