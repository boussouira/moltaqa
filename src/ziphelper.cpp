#include "ziphelper.h"
#include "JlCompress.h"
#include "mainwindow.h"
#include "libraryinfo.h"
#include "utils.h"
#include <qdebug.h>

ZipHelper::ZipHelper(QObject *parent) :
    QObject(parent),
    m_stat(Closed)
{
}

ZipHelper::~ZipHelper()
{
}

void ZipHelper::setPath(const QString &path)
{
    m_zipPath = path;
}

void ZipHelper::open()
{
    m_zip.setZipName(m_zipPath);

    if(!m_zip.open(QuaZip::mdUnzip)) {
        qCritical() << "ZipHelper: open zip file error"
                    << m_zip.getZipError()
                    << "Path" << m_zipPath;
    } else {
        m_stat = Open;
    }
}

void ZipHelper::close()
{
    m_zip.close();
    m_stat = Closed;
}

QString ZipHelper::unzip()
{
    m_unzipDirPath= QFileInfo(m_zipPath).baseName();
    QDir dir(MW->libraryInfo()->tempDir());

    while(dir.exists(m_unzipDirPath))
        m_unzipDirPath.append("_");

    dir.mkdir(m_unzipDirPath);

    m_unzipDirPath = dir.absoluteFilePath(m_unzipDirPath);

    if(unzip(m_zipPath, m_unzipDirPath)) {
        m_stat = UnZipped;
        return m_unzipDirPath;
    } else {
        qCritical() << "ZipHelper: Error when unzip file:" << m_zipPath << "in" << m_unzipDirPath;
        return QString();
    }
}

QString ZipHelper::zip()
{
    if(m_stat != UnZipped) {
        qDebug("ZipHelper: File is already zipped");
        return QString();
    }

    QDir tempDir(MW->libraryInfo()->tempDir());
    QString zipPath = Utils::Rand::fileName(tempDir.absolutePath(), true, "mld", QFileInfo(m_zipPath).baseName()+'_');

    if(zip(m_unzipDirPath, zipPath))
        return zipPath;
    else
        return QString();
}

bool ZipHelper::save()
{
    QString newZip = zip();
    QString backupZip = m_zipPath + ".back";
    if(newZip.size()) {
        if(QFile::exists(backupZip) && !QFile::remove(backupZip))
            qWarning() << "ZipHelper: Can't remove backup file:" << backupZip;
        if(QFile::rename(m_zipPath, backupZip)) {
            if(QFile::copy(newZip, m_zipPath)) {
                qDebug() << "ZipHelper: Saved to:" << m_zipPath;
                return true;
            } else {
                qCritical() << "ZipHelper: Can't copy" << newZip << "to" << m_zipPath;
            }
        } else {
            qCritical() << "ZipHelper: Can't create backup file:" << backupZip;
        }
    } else {
        qCritical() << "ZipHelper: Can't compress temp dir:" << m_unzipDirPath;
    }

    return false;
}

ZipHelper::ZipStat ZipHelper::zipStat()
{
    return m_stat;
}

QFilePtr ZipHelper::getFile(const QString &fileName, QIODevice::OpenModeFlag mode)
{
    QFile *file = 0;

    if(m_stat == UnZipped) {
        QDir dir(m_unzipDirPath);
        QString filePath = dir.filePath(fileName);
        QFileInfo info(filePath);

        if(!dir.exists(info.path()))
            dir.mkpath(info.path());

        file = new QFile(filePath);
        if(!file->open(mode)) {
            qWarning("ZipHelper::getFile Can't open file for writing: %s", qPrintable(file->errorString()));
            ml_delete(file);
        }
    } else {
        qWarning("ZipHelper::getFile Zip file is not in Open stat");
    }

    return QFilePtr(file);
}

QuaZipFilePtr ZipHelper::getZipFile(const QString &fileName)
{
    QuaZipFile *file = 0;

    if(m_stat == Open) {
        if(m_zip.setCurrentFile(fileName)) {
            if(file->open(QIODevice::ReadOnly))
                file = new QuaZipFile(&m_zip);
            else
                qWarning("ZipHelper::getZipFile open error %d", file->getZipError());

        } else {
            qWarning("ZipHelper::getZipFile setCurrentFile error %d", m_zip.getZipError());
        }
/*
    } else if(m_stat == UnZipped) {
        QDir dir(m_unzipDirPath);
        if(dir.exists(fileName)) {
            QFile file(dir.filePath(fileName));
            if(!file.open(QFile::WriteOnly)) {
                qWarning("getZipFile: Can't open file for writing: %s", qPrintable(file.errorString()));
            }

        } else {
            qWarning() << "getZipFile: File doesn't exists:" << dir.filePath(fileName);
        }
    }
*/
    } else {
         qWarning("ZipHelper::getZipFile File is not in Open stat");
    }

    return QuaZipFilePtr(file);
}

XmlDomHelperPtr ZipHelper::getDomHelper(const QString &fileName, const QString &documentName)
{
    XmlDomHelper *dom = 0;
    if(m_stat == Open) {
        QuaZipFile pagesFile(&m_zip);

        if(m_zip.setCurrentFile(fileName)) {
            if(pagesFile.open(QIODevice::ReadOnly)) {
                dom = new XmlDomHelper();
                dom->load(&pagesFile);
            } else {
                qWarning("ZipHelper::getDomHelper open error %d", pagesFile.getZipError());
            }
        }
    } else if(m_stat == UnZipped) {
        QDir dir(m_unzipDirPath);
        dom = new XmlDomHelper();
        dom->setFilePath(dir.filePath(fileName));
        dom->setDocumentName(documentName);

        if(!dir.exists(fileName))
            dom->create();

        dom->load();
        qDebug() << "ZipHelper::getDomHelper Dom path:" << dir.filePath(fileName);
    } else {
        qWarning("ZipHelper::getDomHelper File is not in Open or Unzipped stat");
    }

    return XmlDomHelperPtr(dom);
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

        char buf[4096];
        int len = 0;

        while (!file.atEnd()) {
            len = file.read(buf, 4096);
            out.write(buf, len);
        }

        out.close();
        file.close();
    }

    return true;
}

bool zipDir(const QDir &parentDir, const QString &path, QuaZipFile *outFile)
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

        if(!outFile->open(QIODevice::WriteOnly, QuaZipNewInfo(inFilePath, inFilePath))) {
            qWarning("zipDir: open outFile error: %d", outFile->getZipError());
            return false;
        }

        char buf[4096];
        qint64 l = 0;

        while (!inFile.atEnd()) {
             l = inFile.read(buf, 4096);
            if (l < 0) {
                qWarning("zipDir: input file read error: %s", qPrintable(inFile.errorString()));
                break;
            }
            if (l == 0)
                break;
            if (outFile->write(buf, l) != l) {
                qWarning("zipDir: write chunk error: %d", outFile->getZipError());
                break;
            }
        }

        outFile->close();

        if(outFile->getZipError()!=UNZ_OK) {
            qWarning("zipDir: outFile close error: %d", outFile->getZipError());
            return false;
        }

        inFile.close();
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

    if(!zipDir(inDir, dir, &outFile)) {
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
