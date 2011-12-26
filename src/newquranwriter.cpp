#include "newquranwriter.h"
#include "bookexception.h"
#include "utils.h"
#include "mainwindow.h"
#include "libraryinfo.h"
#include <qsettings.h>
#include <qdir.h>
#include <qdatetime.h>
#include <qdebug.h>
#include <librarybook.h>

newQuranWriter::newQuranWriter()
{
    m_tempFolder = MW->libraryInfo()->tempDir();
}

void newQuranWriter::createNewBook(QString bookPath)
{
    if(bookPath.isEmpty())
        m_bookPath = Utils::genBookName(m_tempFolder, true);
    else
        m_bookPath = bookPath;

    qWarning() << "newQuranWriter File at:" << m_bookPath;
    if(QFile::exists(m_bookPath)) {
        qWarning() << "createNewBook: File at:" << m_bookPath << "already exists";
        if(QFile::remove(m_bookPath))
            qWarning() << "Delete it";
        else
            qWarning("Can't delete it!");
    }

    m_file.setFileName(m_bookPath);
    m_zip.setIoDevice(&m_file);
    if(!m_zip.open(QuaZip::mdCreate)) {
        throw BookException(QObject::tr("لا يمكن انشاء كتاب جديد"), m_bookPath, m_zip.getZipError());
    }
}

QString newQuranWriter::bookPath()
{
    return m_bookPath;
}

void newQuranWriter::startReading()
{
    m_pageId = 0;

    m_pagesDoc.setContent(QString("<?xml version='1.0' encoding='UTF-8'?><pages></pages>"));
    m_pagesElemnent = m_pagesDoc.documentElement();
}

void newQuranWriter::endReading()
{
    QuaZipFile pagesFile(&m_zip);
    if(pagesFile.open(QIODevice::WriteOnly, QuaZipNewInfo("pages.xml"))) {
        QTextStream out(&pagesFile);
        out << m_pagesDoc.toString(2);
    } else {
        qCritical("Can't write to pages.xml - Error: %d", pagesFile.getZipError());
    }

    m_zip.close();
}

void newQuranWriter::addPage(const QString &text, int soraNum, int ayaNum, int pageNum)
{
    m_pageId++;

    QuaZipFile outFile(&m_zip);
    if(outFile.open(QIODevice::WriteOnly,
                     QuaZipNewInfo(QString("pages/p%1.html").arg(m_pageId)))) {
        QTextStream out(&outFile);
        out << text;
    } else {
        qCritical("Can't write to pages/p%d.html - Error: %d", m_pageId, outFile.getZipError());
    }

    QDomElement page = m_pagesDoc.createElement("item");
    page.setAttribute("id", m_pageId);
    page.setAttribute("sora", soraNum);
    page.setAttribute("aya", ayaNum);
    page.setAttribute("page", pageNum);

    m_pagesElemnent.appendChild(page);
}
