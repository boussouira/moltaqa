#ifndef ZIPHELPER_H
#define ZIPHELPER_H

#include <qobject.h>
#include <qfile.h>
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

#include "xmldomhelper.h"

typedef QSharedPointer<QuaZipFile> QuaZipFilePtr;
typedef QSharedPointer<XmlDomHelper> XmlDomHelperPtr;

class ZipHelper : public QObject
{
    Q_OBJECT

public:
    ZipHelper(QObject *parent = 0);
    ~ZipHelper();

    enum OpenMode {
        ReadOnly,
        WriteOnly
    };

    enum ZipStat {
        Open,
        Closed,
        UnZipped
    };

    void setPath(const QString &path);

    void open();
    void close();

    void unzip();
    void zip();

    void save();

    QuaZipFilePtr getZipFile(const QString &fileName);
    XmlDomHelperPtr getDomHelper(const QString &fileName);

protected:
    ZipStat m_stat;
    QString m_zipPath;
    QString m_unzipDirPath;
    QuaZip m_zip;
};

#endif // ZIPHELPER_H
