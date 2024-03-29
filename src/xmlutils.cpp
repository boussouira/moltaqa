#include "xmlutils.h"

#include <qdebug.h>
#include <qfile.h>

namespace Utils {
namespace Xml {

QDomDocument getDomDocument(const QString &filePath)
{

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "getDomDocument: file open error" << file.errorString()
                    << "path" << filePath;
        return QDomDocument();
    }

    return getDomDocument(&file);
}

QDomDocument getDomDocument(QIODevice *file)
{
    QString errorStr;
    int errorLine;
    int errorColumn;

    QDomDocument doc;
    if (!doc.setContent(file, 0, &errorStr, &errorLine, &errorColumn)) {
        qCritical("getDomDocument: Parse error at line %d, column %d: %s",
                  errorLine, errorColumn, qPrintable(errorStr));

        QFile *f = qobject_cast<QFile*>(file);
        if(f)
            qCritical("\tFile: %s", qPrintable(f->fileName()));

        return QDomDocument();
    }

    return doc;
}

QDomElement findChildElement(QDomElement &parent, QDomDocument &doc, const QString &tagName)
{
    QDomElement child = parent.firstChildElement(tagName);
    if(child.isNull())
            child = parent.appendChild(doc.createElement(tagName)).toElement();

    return child;
}

QDomNode findChildText(QDomElement &parent, QDomDocument &doc, bool cdata)
{
    QDomNode child = parent.firstChild();
    if(child.isNull()) {
        if(cdata)
            child = parent.appendChild(doc.createCDATASection(""));
        else
            child = parent.appendChild(doc.createTextNode(""));
    }

    return child;
}

void setElementText(QDomElement &parent, QDomDocument &doc, const QString &text, bool cdata)
{
    if(!parent.isNull()) {
        QDomNode textNode = Utils::Xml::findChildText(parent, doc, cdata);
        if(!textNode.isNull())
            textNode.setNodeValue(text);
        else
            qCritical("findChildText: Text node is null");
    } else {
        qCritical("findChildText: element is null");
    }
}
}
}
