#ifndef XMLUTILS_H
#define XMLUTILS_H

#include "qdom.h"

namespace Utils {

QDomDocument getDomDocument(QString filePath);
QDomDocument getDomDocument(QIODevice *file);

}

#endif // XMLUTILS_H
