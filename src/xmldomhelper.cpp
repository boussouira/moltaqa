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

void XmlDomHelper::setDocumentName(const QString &name)
{
    m_documentName = name;
}

QString XmlDomHelper::documentName()
{
    return m_documentName;
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
    ml_return_on_fail(!m_domLoaded);
    ml_return_on_fail2(m_filePath.size(), "XmlDomHelper::load empty file path");

    if(m_needSave)
        qCritical("XmlDomHelper::load Dom need to be saved first");

    QFile file(m_filePath);
    ml_return_on_fail2(file.open(QIODevice::ReadOnly),
               "XmlDomHelper::load open file error:" << file.errorString() << m_filePath);

    load(&file);
}

void XmlDomHelper::load(QIODevice *file)
{
    ml_return_on_fail(!m_domLoaded);

    m_doc = Utils::Xml::getDomDocument(file);
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
    ml_return_on_fail2(outFile.open(QFile::WriteOnly | QFile::Truncate), "XmlDomHelper::save open error:" << filePath);

    save(&outFile);
}

void XmlDomHelper::save(QIODevice *ioDevice)
{
    QTextStream out(ioDevice);
    m_doc.save(out, 4);

    m_needSave = false;
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
    ml_return_on_fail2(!QFile::exists(m_filePath),
               "XmlDomHelper::create file already exists" << m_filePath);

    QString path = QFileInfo(m_filePath).path();

    if(QFile::exists(path)) {
        QDir dir;
        dir.mkpath(path);
    }

    QFile file(m_filePath);
    ml_return_on_fail2(file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate),
               "XmlDomHelper::create open file error:" << file.errorString());

    ml_return_on_fail(m_documentName.size());

    QTextStream out(&file);
    out.setCodec("utf-8");

    out << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>" << "\n"
        << "<" << m_documentName << ">"
        << "</" << m_documentName << ">";
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

QDomElement elementFind(QDomElement element, const QString &tag, const QString &attr, const QString &value)
{
    while(!element.isNull()) {
        if(element.attribute(attr) == value)
            return element;

        if(element.hasChildNodes()) {
            QDomElement nextElement = element.nextSiblingElement(tag);
            if(!nextElement.isNull()) {
                int next = nextElement.attribute(attr).toInt();
                if(next <= value.toInt()) {
                    element = element.nextSiblingElement(tag);
                    continue;
                }
            }
            QDomElement child = element.firstChildElement(tag);
            while(!child.isNull()){
                QDomElement found = elementFind(child, tag, attr, value);
                if(!found.isNull())
                    return found;

                child = child.nextSiblingElement(tag);
            }
        }

        element = element.nextSiblingElement(tag);
    }

    return QDomElement();
}

QDomElement XmlDomHelper::treeFindElement(const QString &tag, const QString &attr, const QString &value)
{
    return elementFind(m_rootElement.firstChildElement(tag), tag, attr, value);
}

QDomElement XmlDomHelper::setElementText(QDomElement &parent, const QString &tagName, const QString &text, bool cdata)
{
    QDomElement element = Utils::Xml::findChildElement(parent, m_doc, tagName);
    if(!element.isNull()) {
        QDomNode textNode = Utils::Xml::findChildText(element, m_doc, cdata);
        if(!textNode.isNull())
            textNode.setNodeValue(text);
        else
            qCritical("XmlDomHelper::setElementText Text node is null");
    } else {
        qCritical("XmlDomHelper::setElementText element is null");
    }

    return element;
}
