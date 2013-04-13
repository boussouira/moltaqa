#include "newquranwriter.h"
#include "bookexception.h"
#include "utils.h"
#include "mainwindow.h"
#include "libraryinfo.h"
#include "librarybook.h"
#include <qdir.h>
#include <qdatetime.h>

newQuranWriter::newQuranWriter()
{
}

void newQuranWriter::createNewBook()
{
    m_bookPath = Utils::Rand::newBook(MW->libraryInfo()->booksDir());

    while(QFile::exists(m_bookPath)) {
        qWarning() << "newQuranWriter::createNewBook file" << m_bookPath << "already exists";
        m_bookPath.replace(".mlb", "_.mlb");
    }

    m_zip.setZipName(m_bookPath);
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

    m_pagesDoc.setContent(QString("<?xml version=\"1.0\" encoding=\"utf-8\" ?><ayat></ayat>"));
    m_pagesElemnent = m_pagesDoc.documentElement();
}

void newQuranWriter::endReading()
{
    QuaZipFile pagesFile(&m_zip);
    if(pagesFile.open(QIODevice::WriteOnly, QuaZipNewInfo("pages.xml"))) {
        QTextStream out(&pagesFile);
        out.setCodec("utf-8");

        m_pagesDoc.save(out, 1);
    } else {
        throw BookException("newQuranWriter::endReading save DOM error",
                            "pages.xml",
                            pagesFile.getZipError());
    }

    m_zip.close();

    if(m_zip.getZipError()!=0)
        throw BookException("newQuranWriter::endReading zip file close error",
                            m_zip.getZipName(),
                            m_zip.getZipError());
}

void newQuranWriter::addPage(const QString &text, int soraNum, int ayaNum, int pageNum)
{
    m_pageId++;

    QDomElement page = m_pagesDoc.createElement("aya");
    page.setAttribute("id", m_pageId);
    page.setAttribute("sora", soraNum);
    page.setAttribute("aya", ayaNum);
    page.setAttribute("page", pageNum);

    QDomText textNode = m_pagesDoc.createTextNode(text);
    page.appendChild(textNode);

    m_pagesElemnent.appendChild(page);
}
