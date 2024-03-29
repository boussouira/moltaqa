#ifndef ZIPHELPER_H
#define ZIPHELPER_H

#include "quazip.h"
#include "quazipfile.h"

#include <qfile.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>

#include "sqlutils.h"
#include "xmldomhelper.h"

class BookPage;

class SimpleZipWriter {
public:
    SimpleZipWriter();
    ~SimpleZipWriter();

    bool open(QString zipFilePath=QString());
    bool close();

    inline QString zipPath() { return m_zipPath; }

    void add(const QString &fileName, const QByteArray &data);
    void addFromFile(const QString &fileName, const QString &filePath);
    void addFromZip(const QString &filePath);

protected:
    QString m_zipPath;
    QuaZip m_zip;
    bool m_removeZipFile;
};

class ZipWriterManager {
public:
    ZipWriterManager();
    ~ZipWriterManager();

    enum InsertOrder{
        Bottom,
        Top
    };

    bool open(QString zipFilePath=QString());
    bool close();

    QString zipPath() { return m_topdZip.zipPath(); }

    void add(const QString &fileName, const QByteArray &data, InsertOrder order);
    void addFromFile(const QString &fileName, const QString &filePath, InsertOrder order);
    void addFromZip(const QString &filePath, InsertOrder order);

protected:
    void openBottomZip();

protected:
    SimpleZipWriter m_bottomZip;
    SimpleZipWriter m_topdZip;
    bool m_haveBottomZip;
};

class ZipHelper
{
public:

    ZipHelper();
    ~ZipHelper();

    enum InsertOrder{
        Bottom,
        Top
    };

    void open();

    QString datbasePath();

    inline bool transaction() { return m_db.transaction(); }
    inline bool commit() { return m_db.commit(); }

    void add(const QString &filename, const QString &data, InsertOrder order);
    void add(const QString &filename, const QByteArray &data, InsertOrder order);
    void add(const QString &filename, QIODevice *ioDevice, InsertOrder order);
    void addFromFile(const QString &fileName, const QString &filePath, InsertOrder order);
    void addFromDomHelper(const QString &filename, XmlDomHelper& domHelper, InsertOrder order);
    void addFromZip(const QString &filePath);
    void addFromDomDocument(const QString &filename, QDomDocument &doc, InsertOrder order);

    void replace(const QString &filename, const QString &data, InsertOrder order);
    void replace(const QString &filename, QIODevice *ioDevice, InsertOrder order);
    void replaceFromFile(const QString &fileName, const QString &filePath, InsertOrder order);
    void replaceFromDomHelper(const QString &filename, XmlDomHelper& domHelper, InsertOrder order);

    void update(BookPage *page);

    void remove(const QString &filename);

    QString zip(QString zipFilePath=QString());

    static bool unzip(const QString &zipPath, const QString &outPath);
    static bool zip(const QString &dir, const QString &zipPath);

protected:
    void creatDB();

protected:
    QString m_dbPath;
    QSqlDatabase m_db;
    QSqlQuery m_query;
    int m_bottomPos;
    int m_topPos;
};

#endif // ZIPHELPER_H
