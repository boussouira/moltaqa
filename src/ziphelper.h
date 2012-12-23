#ifndef ZIPHELPER_H
#define ZIPHELPER_H

#include <qobject.h>
#include <qfile.h>
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

#include "xmldomhelper.h"

typedef QSharedPointer<QFile> QFilePtr;
typedef QSharedPointer<QuaZipFile> QuaZipFilePtr;

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

    QString unzip();
    QString zip();

    bool save();
    ZipStat zipStat();

    QFilePtr getFile(const QString &fileName, QIODevice::OpenModeFlag mode=QIODevice::WriteOnly);
    QuaZipFilePtr getZipFile(const QString &fileName);
    XmlDomHelper::Ptr getDomHelper(const QString &fileName, const QString &documentName=QString());

    static bool unzip(const QString &zipPath, const QString &outPath);
    static bool zip(const QString &dir, const QString &zipPath);

protected:
    ZipStat m_stat;
    QString m_zipPath;
    QString m_unzipDirPath;
    QuaZip m_zip;
};

#endif // ZIPHELPER_H
