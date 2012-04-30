#include "xmlmanager.h"
#include "xmlutils.h"
#include "utils.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qdebug.h>
#include <qxmlstream.h>

XmlManager::XmlManager(QObject *parent) :
    ListManager(parent)
{
}

void XmlManager::setFilePath(QString path)
{
    m_dom.setFilePath(path);
}

void XmlManager::save(QStandardItemModel *model)
{
    if(m_dom.needSave()) {
        qCritical("XmlManager::save The XML need to be save before saving the model");
        m_dom.setNeedSave(false);
    }

    QFile file(m_dom.filePath());

    ml_return_on_fail2(file.open(QIODevice::WriteOnly), "ListManager::save open file error" << file.errorString());

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);

    saveModel(writer, model);

    file.close();
    reloadModels();
}

void XmlManager::reloadModels()
{
    m_dom.reload();

    ListManager::reloadModels();
}
