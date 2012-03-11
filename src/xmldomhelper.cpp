#include "xmldomhelper.h"
#include "xmlutils.h"
#include <qtextstream.h>

XmlDomHelper::XmlDomHelper() : m_needSave(false)
{
}

XmlDomHelper::~XmlDomHelper()
{
}

void XmlDomHelper::setFilePath(QString &path)
{
    m_filePath = path;
}

void XmlDomHelper::setNeedSave(bool saveDom)
{
    m_needSave = saveDom;
}

void XmlDomHelper::load()
{
    if(!m_filePath.isEmpty()) {
        m_doc = Utils::getDomDocument(m_filePath);
        m_rootElement = m_doc.documentElement();
    } else {
        qCritical("XmlDomHelper: empty file path");
    }
}

void XmlDomHelper::load(QIODevice *file)
{
    m_doc = Utils::getDomDocument(file);
    m_rootElement = m_doc.documentElement();
}

void XmlDomHelper::save()
{
    save(m_filePath);
}

void XmlDomHelper::save(const QString &filePath)
{
    QFile outFile(filePath);
    if (outFile.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&outFile);
        m_doc.save(out, 4);

        m_needSave = false;
    } else {
        qCritical("Can't open file for writing: %s", qPrintable(filePath));
    }
}

void XmlDomHelper::reload()
{
    if(m_needSave)
        save();

    load();
}

QDomElement XmlDomHelper::findElement(const QString &attr, const QString &value)
{
    QString tag;
    return findElement(tag, attr, value);
}

QDomElement XmlDomHelper::findElement(const QString &tag, const QString &attr, const QString &value)
{
    QDomElement e = m_rootElement.firstChildElement(tag);

    while(!e.isNull()) {
        if(e.attribute(attr) == value)
            return e;

        e = e.nextSiblingElement(tag);
    }

    return QDomElement();
}
