#include "ziphelper.h"
#include "JlCompress.h"
#include "mainwindow.h"
#include "libraryinfo.h"

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
        qCritical("ZipHelper: Can't open book at '%s' Error: %d",
                  qPrintable(m_zipPath),
                  m_zip.getZipError());
    } else {
        m_stat = Open;
    }
}

void ZipHelper::close()
{
    m_zip.close();
    m_stat = Closed;
}

void ZipHelper::unzip()
{
    m_unzipDirPath= QFileInfo(m_zipPath).baseName();
    QDir dir(MW->libraryInfo()->tempDir());

    while(dir.exists(m_unzipDirPath))
        m_unzipDirPath.append("_");

    if(JlCompress::extractDir(m_zipPath, m_unzipDirPath).count())
        m_stat = UnZipped;
    else
        qCritical("ZipHelper: Error when unzip file");
}

void ZipHelper::zip()
{
    JlCompress::compressDir(m_zipPath, m_unzipDirPath);
}

void ZipHelper::save()
{
}

QuaZipFilePtr ZipHelper::getZipFile(const QString &fileName)
{
    QuaZipFile *file = 0;

    if(m_zip.setCurrentFile(fileName)) {
        if(file->open(QIODevice::ReadOnly))
            file = new QuaZipFile(&m_zip);
        else
            qWarning("getFileContent: open error %d", file->getZipError());

    } else {
        qWarning("getFileContent: setCurrentFile error %d", m_zip.getZipError());
    }

    return QuaZipFilePtr(file);
}

XmlDomHelperPtr ZipHelper::getDomHelper(const QString &fileName)
{
    XmlDomHelper *dom = 0;
    QuaZipFile pagesFile(&m_zip);

    if(m_zip.setCurrentFile(fileName)) {
        if(pagesFile.open(QIODevice::ReadOnly)) {
            dom = new XmlDomHelper();
            dom->load(&pagesFile);
        } else {
            qWarning("getBookInfo: open error %d", pagesFile.getZipError());
        }
    }

    return XmlDomHelperPtr(dom);
}
