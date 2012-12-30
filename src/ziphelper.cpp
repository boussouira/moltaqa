#include "ziphelper.h"
#include "JlCompress.h"
#include "mainwindow.h"
#include "libraryinfo.h"
#include "utils.h"
#include "ziputils.h"
#include "librarymanager.h"
#include "libraryinfo.h"

/* Simple zip writer */

SimpleZipWriter::SimpleZipWriter()
{
}

SimpleZipWriter::~SimpleZipWriter()
{
    if(m_removeZipFile) {
        if(m_zip.isOpen())
            m_zip.close();

        if(QFile::exists(m_zipPath)) {
            ml_warn_on_fail(QFile::remove(m_zipPath),
                            "SimpleZipWriter: Can't remove temp zip file" << m_zipPath);
        }
    }
}

bool SimpleZipWriter::open(QString zipFilePath)
{
    m_zipPath = zipFilePath.size()
            ? zipFilePath
            : Utils::Rand::fileName(LibraryManager::instance()->libraryInfo()->tempDir(),
                                    true, "temp_zip_", "zip");

    m_zip.setZipName(m_zipPath);
    if(!m_zip.open(QuaZip::mdCreate)) {
        qWarning() << "SimpleZipWriter::open Can't creat zip file:"
                   << m_zipPath << "error:" << m_zip.getZipError();

        return false;
    }

    m_removeZipFile = zipFilePath.isEmpty();

    return true;
}

bool SimpleZipWriter::close()
{
    m_zip.close();

    return !m_zip.getZipError();
}

void SimpleZipWriter::add(const QString &fileName, const QByteArray &data)
{
    QuaZipFile outFile(&m_zip);
    ml_return_on_fail2(outFile.open(QIODevice::WriteOnly, QuaZipNewInfo(fileName)),
                           "SimpleZipWriter::addFromFile targetFile.open error" << outFile.getZipError());

    outFile.write(data);

    outFile.close();
}

void SimpleZipWriter::addFromFile(const QString &fileName, const QString &filePath)
{
    QFile inFile;
    inFile.setFileName(filePath);
    if(!inFile.open(QIODevice::ReadOnly)) {
        qWarning() << "SimpleZipWriter::addFromFile can't open file for reading:"
                   << inFile.errorString();
        return;
    }

    QuaZipFile outFile(&m_zip);
    ml_return_on_fail2(outFile.open(QIODevice::WriteOnly, QuaZipNewInfo(fileName)),
                           "SimpleZipWriter::addFromFile targetFile.open error" << outFile.getZipError());

    Utils::Files::copyData(inFile, outFile);

    outFile.close();
    inFile.close();
}

void SimpleZipWriter::addFromZip(const QString &filePath)
{
    Utils::Zip::copyFromZip(filePath, &m_zip);
}

/* Zip helper */

ZipHelper::ZipHelper()
{
}

ZipHelper::~ZipHelper()
{
    m_remover.removeDatabase(m_db);

    if(QFile::exists(m_dbPath)) {
        ml_warn_on_fail(QFile::remove(m_dbPath),
                        "ZipHelper: Can't remove temp file:" << m_dbPath);
    }
}

void ZipHelper::open()
{
    creatDB();
}

void ZipHelper::creatDB()
{
    m_dbPath = Utils::Rand::fileName(LibraryManager::instance()->libraryInfo()->tempDir(),
                                     true, "temp_zip_", "db");

    QString conn = "ZipHelper." + QFileInfo(m_dbPath).baseName();
    while(m_db.contains(conn))
        conn.append('_');

    m_db = QSqlDatabase::addDatabase("QSQLITE", conn);
    m_db.setDatabaseName(m_dbPath);

    ml_open_db(m_db);

    m_query = QSqlQuery(m_db);

    m_appendPos = 0;
    m_prependPos = 0;

    // Create new table
    QueryBuilder q;
    q.setTableName("files_data");
    q.setIgnoreExistingTable(true);
    q.setQueryType(QueryBuilder::Create);

    q.set("file_pos", "INT");
    q.set("file_name", "TEXT");
    q.set("file_data", "TEXT");

    q.exec(m_query);

    m_query.exec("create unique index file_name_index on files_data (file_name)");
    m_query.exec("create index file_pos_index on files_data (file_pos)");
}

QString ZipHelper::datbasePath()
{
    return m_dbPath;
}

void ZipHelper::add(const QString &filename, const QString &data, InsertOrder order)
{
    add(filename, data.toUtf8(), order);
}

void ZipHelper::add(const QString &filename, const QByteArray &data, ZipHelper::InsertOrder order)
{
    QueryBuilder q;
    q.setTableName("files_data", QueryBuilder::Insert);

    q.set("file_pos", (order==AppendFile ? ++m_appendPos : --m_prependPos));
    q.set("file_name", filename);
    q.set("file_data", data);

    q.exec(m_query);
}

void ZipHelper::add(const QString &filename, QIODevice *ioDevice, InsertOrder order)
{
    QByteArray out;
    char buf[4096];
    int len = 0;

    while (!ioDevice->atEnd()) {
        len = ioDevice->read(buf, 4096);
        out.append(buf, len);

        if(len <= 0)
            break;
    }

    add(filename, out, order);
}

void ZipHelper::addFromFile(const QString &fileName, const QString &filePath, InsertOrder order)
{
    QFile file;
    file.setFileName(filePath);
    if(!file.open(QIODevice::ReadOnly)) {
        qWarning() << "ZipHelper::appendFromFile can't open file for reading:"
                   << file.errorString();
        return;
    }

    add(fileName, &file, order);
}

void ZipHelper::addFromDomHelper(const QString &filename, XmlDomHelper &domHelper, ZipHelper::InsertOrder order)
{
    QString domPath = Utils::Rand::fileName(MW->libraryInfo()->tempDir(),
                                            true, "temp_dom_", "xml");

    domHelper.save(domPath);
    addFromFile(filename, domPath, order);

    QFile::remove(domPath);
}

void ZipHelper::addFromZip(const QString &filePath)
{
    QuaZip zip;
    zip.setZipName(filePath);

    if(!zip.open(QuaZip::mdUnzip)) {
        qCritical() << "ZipHelper::addFromZip open zip file error" << zip.getZipError()
                    << "Path" << filePath;
        return;
    }

    m_db.transaction();

    QuaZipFileInfo info;
    QuaZipFile file(&zip);
    for(bool more=zip.goToFirstFile(); more; more=zip.goToNextFile()) {
        ml_return_on_fail2(zip.getCurrentFileInfo(&info),
                           "getPages: getCurrentFileInfo Error" << zip.getZipError());

        if(!file.open(QIODevice::ReadOnly)) {
            qWarning("ZipHelper::addFromZip zip error %d",
                     zip.getZipError());
            continue;
        }

        add(info.name, &file, AppendFile);

        file.close();

        if(file.getZipError()!=UNZ_OK) {
            qWarning("ZipHelper::addFromZip Unknow zip error %d",
                     file.getZipError());
            continue;
        }
    }

    m_db.commit();
}

void ZipHelper::addFromDomDocument(const QString &filename, QDomDocument &doc, ZipHelper::InsertOrder order)
{
    QString domPath = Utils::Rand::fileName(MW->libraryInfo()->tempDir(),
                                            true, "temp_dom_", "xml");

    QFile file;
    file.setFileName(domPath);
    if(!file.open(QFile::WriteOnly | QFile::Truncate)) {
        qWarning() << "ZipHelper::addFromDomDocument can't open file for writing:"
                   << file.errorString();
        return;
    }

    QTextStream out(&file);
    out.setCodec("utf-8");

    doc.save(out, 4);

    addFromFile(filename, domPath, order);

    QFile::remove(domPath);
}

void ZipHelper::replace(const QString &filename, const QString &data, ZipHelper::InsertOrder order)
{
    remove(filename);
    add(filename, data, order);
}

void ZipHelper::replace(const QString &filename, QIODevice *ioDevice, ZipHelper::InsertOrder order)
{
    remove(filename);
    add(filename, ioDevice, order);
}

void ZipHelper::replaceFromFile(const QString &fileName, const QString &filePath, ZipHelper::InsertOrder order)
{
    remove(fileName);
    addFromFile(fileName, filePath, order);
}

void ZipHelper::replaceFromDomHelper(const QString &filename, XmlDomHelper &domHelper, ZipHelper::InsertOrder order)
{
    remove(filename);
    addFromDomHelper(filename, domHelper, order);
}

void ZipHelper::update(BookPage *page)
{
    QString filename = QString("pages/p%2.html").arg(page->pageID);

    QueryBuilder q;
    q.setTableName("files_data", QueryBuilder::Delete);
    q.where("file_name", filename);

    q.exec(m_query);

    q.setTableName("files_data", QueryBuilder::Insert);
    q.set("file_pos", page->pageID);
    q.set("file_name", filename);
    q.set("file_data", page->text.toUtf8());

    q.exec(m_query);
}

void ZipHelper::remove(const QString &filename)
{
    QueryBuilder q;
    q.setTableName("files_data", QueryBuilder::Delete);
    q.where("file_name", filename);

    q.exec(m_query);
}

QString ZipHelper::zip(QString zipFilePath)
{
    QString zipPath = zipFilePath.size() ? zipFilePath
                                         : Utils::Rand::fileName(MW->libraryInfo()->tempDir(),
                                                                 true, "temp_zip_", "zip");

    QuaZip zip;
    zip.setZipName(zipPath);
    if(!zip.open(QuaZip::mdCreate)) {
        qWarning() << "ZipHelper::zip Can't creat zip file:"
                   << zipPath << "error:" << zip.getZipError();

        return QString();
    }

    QueryBuilder q;
    q.setTableName("files_data", QueryBuilder::Select);
    q.select("file_name");
    q.select("file_data");

    q.orderBy("file_pos");

    q.exec(m_query);

    while(m_query.next()) {
        QString filename = m_query.value(0).toString();
        QuaZipFile outFile(&zip);

        if(!outFile.open(QIODevice::WriteOnly, QuaZipNewInfo(filename))) {
            qWarning() << "ZipHelper::zip Can't add zip file:"
                       << filename << "error:" << outFile.getZipError();

            return QString();
        }

        QByteArray data = m_query.value(1).toByteArray();
        Utils::Files::copyData(data, outFile);
    }

    zip.close();

    ml_return_val_on_fail2(zip.getZipError()==0,
                           "ZipHelper::zip zip file close error" << zip.getZipError(),
                           QString());

    return zipPath;
}



bool ZipHelper::unzip(const QString &zipPath, const QString &outPath)
{
    ml_return_val_on_fail2(QFile::exists(zipPath),
                   "ZipHelper::unzip zip file doesn't exists:" << zipPath, false);
    ml_return_val_on_fail2(QFile::exists(outPath),
                   "ZipHelper::unzip out path doesn't exists:" << outPath, false);

    QDir outDir(outPath);

    QFile zipFile(zipPath);
    QuaZip zip(&zipFile);

    ml_return_val_on_fail2(zip.open(QuaZip::mdUnzip),
                   "ZipHelper::unzip open zip error" << zip.getZipError(), false);

    QuaZipFileInfo info;
    QuaZipFile file(&zip);

    for(bool more=zip.goToFirstFile(); more; more=zip.goToNextFile()) {
        if(!zip.getCurrentFileInfo(&info)) {
            qWarning("ZipHelper::unzip getCurrentFileInfo error %d", zip.getZipError());
            continue;
        }

        if(!file.open(QIODevice::ReadOnly)) {
            qWarning("ZipHelper::unzip open input file error %d", zip.getZipError());
            continue;
        }

        QString outPath = outDir.filePath(info.name);

        outDir.mkpath(QFileInfo(outPath).path());

        QFile out(outPath);
        if(!out.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            qWarning() << "ZipHelper::unzip open output file error" << zip.getZipError()
                       << "Path:" << outPath;

            file.close();
            continue;
        }

        Utils::Files::copyData(file, out);

        out.close();
        file.close();
    }

    return true;
}

bool zipDir(const QDir &parentDir, const QString &path, QuaZipFile &outFile)
{
    QFile inFile;
    QDir bookDir(path);
    QFileInfoList files = bookDir.entryInfoList(QDir::AllDirs|QDir::Files|QDir::NoSymLinks|QDir::NoDotAndDotDot);

    if(files.isEmpty()) {
        qWarning() << "zipDir: input directory is empty" << path;
    }

    foreach(QFileInfo file, files) {
        if(file.isDir()) {
            if(zipDir(parentDir, file.filePath(), outFile))
                continue;
            else
                return false;
        }

        if(!file.isFile()) {
            qWarning() << "zipDir: Can't compress:" << file.filePath();
            continue;
        }

        QString inFilePath = parentDir.relativeFilePath(file.filePath());

        inFile.setFileName(file.filePath());

        if(!inFile.open(QIODevice::ReadOnly)) {
            qWarning("zipDir: open input file error: %s", qPrintable(inFile.errorString()));
            return false;
        }

        if(!outFile.open(QIODevice::WriteOnly, QuaZipNewInfo(inFilePath, inFilePath))) {
            qWarning("zipDir: open outFile error: %d", outFile.getZipError());
            return false;
        }

        Utils::Files::copyData(inFile, outFile);

        outFile.close();
        inFile.close();

        if(outFile.getZipError()!=UNZ_OK) {
            qWarning("zipDir: outFile close error: %d", outFile.getZipError());
            return false;
        }
    }

    return true;
}

bool ZipHelper::zip(const QString &dir, const QString &zipPath)
{
    QFile zipFile(zipPath);
    QDir inDir(dir);

    QuaZip zip(&zipFile);
    if(!zip.open(QuaZip::mdCreate)) {
        qWarning("zip: open zip error: %d", zip.getZipError());
        return false;
    }

    QuaZipFile outFile(&zip);

    if(!zipDir(inDir, dir, outFile)) {
        zip.close();
        return false;
    }

    zip.close();

    if(zip.getZipError()!=0) {
        qWarning("zip: close zip error: %d", zip.getZipError());
        return false;
    }

    return true;
}
