#include "abstractbookreader.h"
#include "bookreaderhelper.h"
#include "clucenequery.h"
#include "clutils.h"
#include "librarybook.h"
#include "libraryinfo.h"
#include "librarymanager.h"
#include "mainwindow.h"
#include "textformatter.h"
#include "utils.h"
#include "xmlutils.h"

#include <qstandarditemmodel.h>
#include <qstringlistmodel.h>
#include <qxmlstream.h>

AbstractBookReader::AbstractBookReader(QObject *parent) : QObject(parent)
{
    m_currentPage = new BookPage();
    m_libraryManager = LibraryManager::instance();
    m_pagesLoaded = false;

    m_query = 0;
    m_highlightPageID = -1;

    m_removeTashekil = Utils::Settings::get("Style/removeTashekil", false).toBool();
}

AbstractBookReader::~AbstractBookReader()
{
    ml_delete_check(m_currentPage);

    if(m_zip.isOpen())
        m_zip.close();
}

void AbstractBookReader::openBook()
{
    ml_return_on_fail2(m_book, "AbstractBookReader::openBook book is null");

    if(!QFile::exists(m_book->path)) {
        throw BookException(tr("لم يتم العثور على ملف الكتاب"), book()->path);
    }

    ZipOpener opener(this);

    connected();
    getBookInfo();
}

void AbstractBookReader::openZip()
{
    m_zip.setZipName(m_book->path);

    if(!m_zip.open(QuaZip::mdUnzip)) {
        qCritical() << "AbstractBookReader::openZip open book error"
                    << m_zip.getZipError() << "\n"
                    << "Book id:" << m_book->id << "\n"
                    << "Title:" << m_book->title << "\n"
                    << "Path:" << m_book->path;
    }
}

void AbstractBookReader::closeZip()
{
    m_zip.close();
}

void AbstractBookReader::setBookInfo(LibraryBook::Ptr bi)
{
    m_book = bi;
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

void AbstractBookReader::loadPages()
{
    ml_return_on_fail(!m_book->isQuran());

    ZipOpener opener(this);

    QuaZipFileInfo info;
    QuaZipFile file(&m_zip);
    for(bool more=m_zip.goToFirstFile(); more; more=m_zip.goToNextFile()) {
        ml_return_on_fail2(m_zip.getCurrentFileInfo(&info), "getPages: getCurrentFileInfo Error" << m_zip.getZipError());

        int id = 0;
        QString name = info.name;
        if(name.startsWith(QLatin1String("pages/p"))) {
            name = name.remove(0, 7);
            name = name.remove(".html");

            bool ok;
            id = name.toInt(&ok);
            if(!ok) {
                qDebug("can't convert '%s' to int", qPrintable(name));
                continue;
            }
        } else {
            continue;
        }

        if(!file.open(QIODevice::ReadOnly)) {
            qWarning("TextBookReader::getPages zip error %d", m_zip.getZipError());
            continue;
        }

        QByteArray out;
        if(!Utils::Files::copyData(file, out))
            break;

        m_pages.insert(id, out);

        file.close();
        if(file.getZipError()!=UNZ_OK) {
            qWarning("TextBookReader::getPages Unknow zip error %d", file.getZipError());
            continue;
        }
    }

    m_pagesLoaded = true;
}

LibraryBook::Ptr AbstractBookReader::book()
{
    return m_book;
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

int AbstractBookReader::nextPageID()
{
    QDomElement element = m_pagesDom.currentElement().isNull()
            ? m_pagesDom.rootElement().firstChildElement()
            : m_pagesDom.currentElement().nextSiblingElement();

    return element.attribute("id").toInt();
}

int AbstractBookReader::prevPageID()
{
    QDomElement element = m_pagesDom.currentElement().isNull()
            ? m_pagesDom.rootElement().lastChildElement()
            : m_pagesDom.currentElement().previousSiblingElement();

    return element.attribute("id").toInt();
}

int AbstractBookReader::pagesCount()
{
    return m_pagesDom.rootElement().childNodes().size();
}

void AbstractBookReader::highlightPage(int pageID, CLuceneQuery *query)
{
    m_query = query;
    m_highlightPageID = pageID;
}

void AbstractBookReader::setRemoveTashkil(bool remove)
{
    m_removeTashekil = remove;
}

QString AbstractBookReader::getFileContent(QString fileName)
{
    ZipOpener opener(this);
    return getFileContent(&m_zip, fileName);
}

QString AbstractBookReader::getPageText(int pageID)
{
    if(m_pagesLoaded) {
        QString text = QString::fromUtf8(m_pages[pageID]);
        return text;
    } else {
        ZipOpener opener(this);
        return getFileContent(&m_zip, QString("pages/p%1.html").arg(pageID));
    }
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

QString AbstractBookReader::getPageText(QuaZip *zip, int pageID)
{
    return getFileContent(zip, QString("pages/p%1.html").arg(pageID));
}

void AbstractBookReader::getBookInfo()
{
   QuaZipFile pagesFile(&m_zip);

   ml_return_on_fail2(m_zip.setCurrentFile("pages.xml"), "getBookInfo: setCurrentFile error:" << m_zip.getZipError());
   ml_return_on_fail2(pagesFile.open(QIODevice::ReadOnly), "getBookInfo: open error" << pagesFile.getZipError());

   m_pagesDom.load(&pagesFile);
}
