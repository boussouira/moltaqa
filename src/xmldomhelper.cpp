#include "xmldomhelper.h"
#include "xmlutils.h"
#include "utils.h"
#include <qtextstream.h>
#include <qdebug.h>

XmlDomHelper::XmlDomHelper() :
    m_needSave(false),
    m_domLoaded(false)
{
}

XmlDomHelper::~XmlDomHelper()
{
    if(m_needSave)
        save();
}

void XmlDomHelper::setFilePath(const QString &path)
{
    m_filePath = path;
}

QString XmlDomHelper::filePath()
{
    return m_filePath;
}

void XmlDomHelper::setNeedSave(bool saveDom)
{
    m_needSave = saveDom;
}

bool XmlDomHelper::needSave()
{
    return m_needSave;
}

void XmlDomHelper::load()
{
    ML_RETURN(m_domLoaded);

    if(!m_filePath.isEmpty()) {

        if(m_needSave)
            qCritical("XmlDomHelper: Dom need to be saved before loading from file");

        QFile file(m_filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            qCritical() << "File open error:" << m_filePath;
            return;
        }

        load(&file);
    } else {
        qCritical("XmlDomHelper: empty file path");
    }
}

void XmlDomHelper::load(QIODevice *file)
{
    ML_RETURN(m_domLoaded);

    m_doc = Utils::getDomDocument(file);
    m_rootElement = m_doc.documentElement();

    m_needSave = false;
    m_domLoaded = true;
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

    m_domLoaded = false;

    load();
}

void XmlDomHelper::create()
{
    ML_RETURN(QFile::exists(m_filePath));

    QString path = QFileInfo(m_filePath).path();

    if(QFile::exists(path)) {
        QDir dir;
        dir.mkpath(path);
    }

    QFile file(m_filePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        qWarning("Can't create file %s: %s", qPrintable(m_filePath), qPrintable(file.errorString()));
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

void XmlDomHelper::setElementText(QDomElement &parent, const QString &tagName, const QString &text, bool cdata)
{
    QDomElement element = Utils::findChildElement(parent, m_doc, tagName);
    if(!element.isNull()) {
        QDomNode textNode = Utils::findChildText(element, m_doc, cdata);
        if(!textNode.isNull())
            textNode.setNodeValue(text);
        else
            qCritical("setElementText: Text node is null");
    } else {
        qCritical("setElementText: element is null");
    }
}
