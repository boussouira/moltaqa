#include "abstractbookreader.h"
#include "libraryinfo.h"
#include "librarybook.h"
#include "bookexception.h"
#include "textformatter.h"
#include "utils.h"
#include "mainwindow.h"
#include "bookreaderhelper.h"
#include "xmlutils.h"

#include <qstandarditemmodel.h>
#include <qstringlistmodel.h>
#include <qxmlstream.h>
#include <QTime>
#include <QDebug>

AbstractBookReader::AbstractBookReader(QObject *parent) : QObject(parent)
{
    m_indexModel = 0;
    m_currentPage = new BookPage();
    m_libraryManager = LibraryManager::instance();
}

AbstractBookReader::~AbstractBookReader()
{
    ml_delete_check(m_indexModel);
    ml_delete_check(m_currentPage);

    if(m_zip.isOpen())
        m_zip.close();
}

void AbstractBookReader::openBook()
{
    ml_return_on_fail2(m_bookInfo, "AbstractBookReader::openBook book is null");

    if(!QFile::exists(m_bookInfo->path)) {
        throw BookException(tr("لم يتم العثور على ملف الكتاب"), bookInfo()->path);
    }

    ZipOpener opener(this);

    connected();
    getBookInfo();
}

void AbstractBookReader::openZip()
{
    m_zip.setZipName(m_bookInfo->path);

    if(!m_zip.open(QuaZip::mdUnzip)) {
        qCritical() << "AbstractBookReader::openZip open book error"
                    << m_zip.getZipError();
        qCritical() << "\t" "id" << m_bookInfo->id
                    << "\t" "title" << m_bookInfo->title
                    << "\t" "Path" << m_bookInfo->path;
    }
}

void AbstractBookReader::closeZip()
{
    m_zip.close();
}

void AbstractBookReader::setBookInfo(LibraryBookPtr bi)
{
    m_bookInfo = bi;
}

void AbstractBookReader::nextAya()
{
}

void AbstractBookReader::prevAya()
{
}

void AbstractBookReader::connected()
{
}

QDomElement AbstractBookReader::getPage(int pid)
{
    return m_pagesDom.findElement("id", pid);
}

QDomElement AbstractBookReader::getPageId(int page, int part)
{
    QString pageNum = QString::number(page);
    QString partNum = QString::number(part);

    QDomElement e = m_pagesDom.rootElement().firstChildElement();
    while(!e.isNull()) {

        if(pageNum == e.attribute("page") && partNum == e.attribute("part")) {
            return e;
        }

        e = e.nextSiblingElement();
    }

    return QDomElement();
}

QDomElement AbstractBookReader::getPageId(int haddit)
{
    return m_pagesDom.findElement("haddit", haddit);
}

QDomElement AbstractBookReader::getQuranPageId(int sora, int aya)
{
    QString soraNum = QString::number(sora);
    QString ayaNum = QString::number(aya);

    QDomElement e = m_pagesDom.rootElement().firstChildElement();
    while(!e.isNull()) {

        if(ayaNum == e.attribute("aya") && soraNum == e.attribute("sora")) {
            return e;
        }

        e = e.nextSiblingElement();
    }

    return QDomElement();
}

LibraryBookPtr AbstractBookReader::bookInfo()
{
    return m_bookInfo;
}

BookPage * AbstractBookReader::page()
{
    return m_currentPage;
}

void AbstractBookReader::goToPage(int pid)
{
    if(pid == -1) {
        firstPage();
    } else if(pid == -2) {
        lastPage();
    } else {
        QDomElement e = getPage(pid);

        if(!e.isNull())
            setCurrentPage(e);
    }
}

void AbstractBookReader::goToPage(int page, int part)
{
    QDomElement e = getPageId(page, part);

    if(!e.isNull())
        setCurrentPage(e);
}

void AbstractBookReader::goToSora(int sora, int aya)
{
    QDomElement e = getQuranPageId(sora, aya);

    if(!e.isNull())
        setCurrentPage(e);
}

void AbstractBookReader::goToHaddit(int hadditNum)
{
    QDomElement e = getPageId(hadditNum);

    if(!e.isNull())
        setCurrentPage(e);
}

void AbstractBookReader::firstPage()
{
    QDomElement e = m_pagesDom.rootElement().firstChildElement();

    if(!e.isNull())
        setCurrentPage(e);
}

void AbstractBookReader::lastPage()
{
    QDomElement e = m_pagesDom.rootElement().lastChildElement();

    if(!e.isNull())
        setCurrentPage(e);
}

void AbstractBookReader::nextPage()
{
    QDomElement e = m_pagesDom.currentElement().isNull()
            ? m_pagesDom.rootElement().firstChildElement()
            : m_pagesDom.currentElement().nextSiblingElement();

    if(!e.isNull())
        setCurrentPage(e);
}

void AbstractBookReader::prevPage()
{
    QDomElement e = m_pagesDom.currentElement().isNull()
            ? m_pagesDom.rootElement().lastChildElement()
            : m_pagesDom.currentElement().previousSiblingElement();

    if(!e.isNull())
        setCurrentPage(e);
}

bool AbstractBookReader::hasNext()
{
    return m_pagesDom.currentElement().isNull()
            ? !m_pagesDom.rootElement().firstChildElement().isNull()
            : !m_pagesDom.currentElement().nextSibling().isNull();
}

bool AbstractBookReader::hasPrev()
{
    return m_pagesDom.currentElement().isNull()
            ? !m_pagesDom.rootElement().lastChildElement().isNull()
            : !m_pagesDom.currentElement().previousSiblingElement().isNull();
}

QString AbstractBookReader::getFileContent(QuaZip *zip, QString fileName)
{
    QuaZipFile file(zip);

    if(zip->setCurrentFile(fileName)) {
        if(file.open(QIODevice::ReadOnly)) {
            return _u(file.readAll());
        } else {
            qWarning("AbstractBookReader::getFileContent: open error %d", file.getZipError());
        }
    } else {
        qWarning("AbstractBookReader::getFileContent: setCurrentFile error %d", zip->getZipError());
    }

    return QString();
}

void AbstractBookReader::getBookInfo()
{
   QuaZipFile pagesFile(&m_zip);

   ml_return_on_fail2(m_zip.setCurrentFile("pages.xml"), "getBookInfo: setCurrentFile error:" << m_zip.getZipError());
   ml_return_on_fail2(pagesFile.open(QIODevice::ReadOnly), "getBookInfo: open error" << pagesFile.getZipError());

   m_pagesDom.load(&pagesFile);
}
