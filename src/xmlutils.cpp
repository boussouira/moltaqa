#include "xmlutils.h"

#include <qfile.h>
#include <qdebug.h>

QDomDocument Utils::getDomDocument(QString filePath)
{

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "File open error:" << filePath;
        return QDomDocument();
    }

    return getDomDocument(&file);
}

QDomDocument Utils::getDomDocument(QIODevice *file)
{
    QString errorStr;
    int errorLine;
    int errorColumn;

    QDomDocument doc;
    if (!doc.setContent(file, 0, &errorStr, &errorLine, &errorColumn)) {
        qCritical("loadModel: Parse error at line %d, column %d: %s",
                  errorLine, errorColumn, qPrintable(errorStr));

        QFile *f = qobject_cast<QFile*>(file);
        if(f)
            qCritical("\tFile: %s", qPrintable(f->fileName()));

        return QDomDocument();
    }

    return doc;
}
