#ifndef XMLUTILS_H
#define XMLUTILS_H

#include <qdom.h>

namespace Utils {
namespace Xml {

QDomDocument getDomDocument(const QString &filePath);
QDomDocument getDomDocument(QIODevice *file);

QDomNode findChildText(QDomElement &parent, QDomDocument &doc, bool cdata=false);
QDomElement findChildElement(QDomElement &parent, QDomDocument &doc, const QString &tagName);

}
}

#endif // XMLUTILS_H
