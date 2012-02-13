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
    if(m_saveDom)
        saveXmlDom();

    clear();
}

void ListManager::setFilePath(QString path)
{
    m_filePath = path;
}

void ListManager::loadXmlDom()
{
    if(m_filePath.isEmpty()) {
        qCritical("loadXmlDom: File path is empty");
        return;
    }

    if(!m_domLoaded) {
        m_doc = Utils::getDomDocument(m_filePath);
        m_rootElement = m_doc.documentElement();

        m_domLoaded = true;
        m_saveDom = false;
    }
}

void ListManager::saveXmlDom()
{
    QFile outFile(m_filePath);
    if (outFile.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&outFile);
        m_doc.save(out, 4);

        m_saveDom = false;
    } else {
        qCritical() << "Can't open file for writing:" << m_filePath;
    }
}

void ListManager::reloadXmlDom()
{
    if(m_saveDom)
        saveXmlDom();

    m_domLoaded = false;
    m_doc.clear();

    loadXmlDom();
}

void ListManager::clear()
{
}

void ListManager::save(QStandardItemModel *model)
{
    if(m_saveDom)
        qCritical("ListManager: The XML need to be save before saving the model");

    QFile file(m_filePath);

    if(!file.open(QIODevice::WriteOnly)) {
        qCritical() << "Can't open file" << m_filePath;
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
    reloadXmlDom();
    clear();
    loadModels();
}
