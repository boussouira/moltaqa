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
    ML_ASSERT(!m_domLoaded);
    ML_ASSERT2(!m_filePath.isEmpty(), "XmlDomHelper::load empty file path");

    if(m_needSave)
        qCritical("XmlDomHelper: Dom need to be saved before loading from file");

    QFile file(m_filePath);
    ML_ASSERT2(file.open(QIODevice::ReadOnly),
               "XmlDomHelper::load open file error:" << file.errorString() << m_filePath);

    load(&file);
}

void XmlDomHelper::load(QIODevice *file)
{
    ML_ASSERT(!m_domLoaded);

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
    ML_ASSERT2(outFile.open(QFile::WriteOnly | QFile::Truncate), "XmlDomHelper::save open error:" << filePath);

    QTextStream out(&outFile);
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
    qDebug("Create empty File...");
    ML_ASSERT2(!QFile::exists(m_filePath),
               "XmlDomHelper::create file already exists" << m_filePath);

    QString path = QFileInfo(m_filePath).path();

    if(QFile::exists(path)) {
        QDir dir;
        dir.mkpath(path);
    }

    QFile file(m_filePath);
    ML_ASSERT2(file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate),
               "XmlDomHelper::create open file error:" << file.errorString());

    ML_ASSERT(!m_documentName.isEmpty());
    qDebug("Create XML File...");

    QTextStream out(&file);
    out.setCodec("utf-8");

    out << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>" << "\n"
        << "<" << m_documentName << ">"
        << "</" << m_documentName << ">";
}

QDomElement XmlDomHelper::findElement(const QString &attr, const QString &value)
{
    return findElement(QString(), attr, value);
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
    QDomElement element = Utils::Xml::findChildElement(parent, m_doc, tagName);
    if(!element.isNull()) {
        QDomNode textNode = Utils::Xml::findChildText(element, m_doc, cdata);
        if(!textNode.isNull())
            textNode.setNodeValue(text);
        else
            qCritical("setElementText: Text node is null");
    } else {
        qCritical("setElementText: element is null");
    }
}
