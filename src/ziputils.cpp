#include "ziputils.h"
#include "ziphelper.h"
#include "utils.h"

namespace Utils
{

namespace Zip {

bool unzip(const QString &zipPath, const QString &outPath)
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

        if(outFile.getZipError()!=UNZ_OK) {
            qWarning("zipDir: outFile close error: %d", outFile.getZipError());
            return false;
        }

        inFile.close();
    }

    return true;
}

bool zip(const QString &dir, const QString &zipPath)
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

bool copyFromZip(const QString &sourcZipPath, QuaZip *targetZip)
{
    QuaZip sourceZip;
    sourceZip.setZipName(sourcZipPath);

    if(!sourceZip.open(QuaZip::mdUnzip)) {
        qCritical() << "ZipHelper::addFromZip open zip file error"
                    << sourceZip.getZipError() << "Path" << sourcZipPath;
        return false;
    }

    QuaZipFileInfo sourceInfo;
    QuaZipFile sourceFile(&sourceZip);
    for(bool more=sourceZip.goToFirstFile(); more; more=sourceZip.goToNextFile()) {
        ml_return_val_on_fail2(sourceZip.getCurrentFileInfo(&sourceInfo),
                               "copyFromZip: sourceZip.getCurrentFileInfo error" << sourceZip.getZipError(),
                               false);

        ml_return_val_on_fail2(sourceFile.open(QIODevice::ReadOnly),
                               "copyFromZip: sourceFile.open error" << sourceZip.getZipError(),
                               false);

        QuaZipFile targetFile(targetZip);
        ml_return_val_on_fail2(targetFile.open(QIODevice::WriteOnly, QuaZipNewInfo(sourceInfo.name)),
                               "copyFromZip: targetFile.open error" << targetFile.getZipError(),
                               false);

        Utils::Files::copyData(sourceFile, targetFile);

        targetFile.close();
        sourceFile.close();

        ml_return_val_on_fail2(sourceFile.getZipError()==UNZ_OK,
                               "copyFromZip: sourceFile.getZipError error" << sourceFile.getZipError(),
                               false);
    }

    return true;
}

}

}
