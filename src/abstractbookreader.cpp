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
    m_bookInfo = 0;
    m_currentPage = new BookPage();
    m_libraryManager = LibraryManager::instance();
}

AbstractBookReader::~AbstractBookReader()
{
    if(m_indexModel)
        delete m_indexModel;

    if(m_currentPage)
        delete m_currentPage;

    if(m_zip.isOpen())
        m_zip.close();
}

void AbstractBookReader::openBook()
{
    Q_CHECK_PTR(m_bookInfo);

    if(!QFile::exists(m_bookInfo->bookPath)) {
        throw BookException(tr("لم يتم العثور على ملف الكتاب"), bookInfo()->bookPath);
    }

    m_zip.setZipName(m_bookInfo->bookPath);

    if(!m_zip.open(QuaZip::mdUnzip)) {
        throw BookException(tr("لا يمكن فتح ملف الكتاب"), m_bookInfo->bookPath, m_zip.getZipError());
    }

    connected();
    getBookInfo();
}

void AbstractBookReader::setBookInfo(LibraryBook *bi)
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
    QDomElement e = m_rootElement.firstChildElement();

    while(!e.isNull()) {
        int pageID = e.attribute("id").toInt();

        if(pageID == pid) {
            return e;
        }

        e = e.nextSiblingElement();
    }

    return QDomElement();
}

QDomElement AbstractBookReader::getPageId(int page, int part)
{
    QString pageNum = QString::number(page);
    QString partNum = QString::number(part);

    QDomElement e = m_rootElement.firstChildElement();
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
    QString hadditNum = QString::number(haddit);

    QDomElement e = m_rootElement.firstChildElement();
    while(!e.isNull()) {

        if(hadditNum == e.attribute("haddit")) {
            return e;
        }

        e = e.nextSiblingElement();
    }

    return QDomElement();
}

QDomElement AbstractBookReader::getQuranPageId(int sora, int aya)
{
    QString soraNum = QString::number(sora);
    QString ayaNum = QString::number(aya);

    QDomElement e = m_rootElement.firstChildElement();
    while(!e.isNull()) {

        if(soraNum == e.attribute("sora") && ayaNum == e.attribute("aya")) {
            return e;
        }

        e = e.nextSiblingElement();
    }

    return QDomElement();
}

LibraryBook *AbstractBookReader::bookInfo()
{
    return m_bookInfo;
}

BookPage * AbstractBookReader::page()
{
    return m_currentPage;
}

void AbstractBookReader::goToPage(int pid)
{
    QDomElement e = getPage(pid);

    if(!e.isNull())
        setCurrentPage(e);
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
    QDomElement e = m_rootElement.firstChildElement();

    if(!e.isNull())
        setCurrentPage(e);
}

void AbstractBookReader::lastPage()
{
    QDomElement e = m_rootElement.lastChildElement();

    if(!e.isNull())
        setCurrentPage(e);
}

void AbstractBookReader::nextPage()
{
    QDomElement e = m_currentElement.nextSiblingElement();

    if(!e.isNull())
        setCurrentPage(e);
}

void AbstractBookReader::prevPage()
{
    QDomElement e = m_currentElement.previousSiblingElement();

    if(!e.isNull())
        setCurrentPage(e);
}

bool AbstractBookReader::hasNext()
{
    return !m_currentElement.nextSibling().isNull();
}

bool AbstractBookReader::hasPrev()
{
    return !m_currentElement.previousSibling().isNull();
}

bool AbstractBookReader::getBookPage(LibraryBook *book, BookPage *page)
{
    if(!book) {
        qWarning("getBookPage: No book with given id");
        return false;
    }

    if(!QFile::exists(book->bookPath)) {
        qWarning() << "File doesn't exists:" << book->bookPath;
        return false;
    }

    QFile zipFile(book->bookPath);
    QuaZip zip(&zipFile);

    if(!zip.open(QuaZip::mdUnzip)) {
        qWarning() << "Can't open zip file:" << book->bookPath << "- Error:" << zip.getZipError();
    }

    if(book->isNormal())
        return getSimpleBookPage(&zip, book, page);
    else if(book->isTafessir())
        return getTafessirPage(&zip, book, page);
    else if(book->isQuran())
        return getQuranPage(&zip, book, page);
    else
        qWarning("getBookPage: Unknow book type");

    zip.close();

    return false;
}

bool AbstractBookReader::getSimpleBookPage(QuaZip *zip, LibraryBook *book, BookPage *page)
{
    Q_UNUSED(book);

    // Get the page
    QuaZipFile pagesFile(zip);
    if(zip->setCurrentFile("pages.xml")) {
        if(!pagesFile.open(QIODevice::ReadOnly)) {
            qWarning("getSimpleBookPage: open error %d", pagesFile.getZipError());
            return false;
        }
    }

    QString pid = QString::number(page->pageID);

    QXmlStreamReader bookReader(&pagesFile);
    while(!bookReader.atEnd()) {
        bookReader.readNext();

        if(bookReader.tokenType() == QXmlStreamReader::StartElement) {
            if(bookReader.name() == "item") {
                if(pid == bookReader.attributes().value("id")) {
                    page->part = bookReader.attributes().value("part").toString().toInt();
                    page->page = bookReader.attributes().value("page").toString().toInt();

                    page->text = getPageText(zip, page->pageID);

                    break;
                }
            }
        }

        if(bookReader.hasError()) {
            qWarning() << "getSimpleBookPage: bookReader error:" << bookReader.errorString();
            break;
        }
    }

    pagesFile.close();

    // Get the title
    QuaZipFile titleFile(zip);

    if(zip->setCurrentFile("titles.xml")) {
        if(!titleFile.open(QIODevice::ReadOnly)) {
            qWarning("getSimpleBookPage: open error: %d", titleFile.getZipError());
            return false;
        }
    }

    QString tid = QString::number(page->titleID);
    QXmlStreamReader titleReader(&titleFile);
    while(!titleReader.atEnd()) {
        titleReader.readNext();

        if(titleReader.tokenType() == QXmlStreamReader::StartElement) {
            if(titleReader.name() == "item") {
                if(tid == titleReader.attributes().value("pageID")) {
                    page->title = titleReader.attributes().value("text").toString();
                    break;
                }
            }
        }

        if(titleReader.hasError()) {
            qWarning() << "getSimpleBookPage: QXmlStreamReader error:" << titleReader.errorString();
            break;
        }
    }

    titleFile.close();

    return true;
}

bool AbstractBookReader::getTafessirPage(QuaZip *zip, LibraryBook *book, BookPage *page)
{
    Q_UNUSED(book);

    // Get the page
    QuaZipFile pagesFile(zip);
    if(zip->setCurrentFile("pages.xml")) {
        if(!pagesFile.open(QIODevice::ReadOnly)) {
            qWarning("getTafessirPage: open error %d", pagesFile.getZipError());
            return false;
        }
    }

    QString pid = QString::number(page->pageID);

    QXmlStreamReader bookReader(&pagesFile);
    while(!bookReader.atEnd()) {
        bookReader.readNext();

        if(bookReader.tokenType() == QXmlStreamReader::StartElement) {
            if(bookReader.name() == "item") {
                if(pid == bookReader.attributes().value("id")) {
                    page->part = bookReader.attributes().value("part").toString().toInt();
                    page->page = bookReader.attributes().value("page").toString().toInt();
                    page->sora = bookReader.attributes().value("sora").toString().toInt();
                    page->aya = bookReader.attributes().value("aya").toString().toInt();

                    page->text = getPageText(zip, page->pageID);

                    break;
                }
            }
        }

        if(bookReader.hasError()) {
            qWarning() << "getTafessirPage: bookReader error:" << bookReader.errorString();
            break;
        }
    }

    pagesFile.close();

    // Get the title
    QuaZipFile titleFile(zip);

    if(zip->setCurrentFile("titles.xml")) {
        if(!titleFile.open(QIODevice::ReadOnly)) {
            qWarning("getTafessirPage: open error: %d", titleFile.getZipError());
            return false;
        }
    }

    QString tid = QString::number(page->titleID);
    QXmlStreamReader titleReader(&titleFile);
    while(!titleReader.atEnd()) {
        titleReader.readNext();

        if(titleReader.tokenType() == QXmlStreamReader::StartElement) {
            if(titleReader.name() == "item") {
                if(tid == titleReader.attributes().value("pageID")) {
                    page->title = titleReader.attributes().value("text").toString();

                    if(page->title.size() < 9) {
                        QuranSora *quranSora = MW->readerHelper()->getQuranSora(page->sora);
                        if(quranSora)
                            page->title = tr("تفسير سورة %1، الاية %2").arg(quranSora->name).arg(page->aya);
                    }

                    break;
                }
            }
        }

        if(titleReader.hasError()) {
            qWarning() << "getTafessirPage: QXmlStreamReader error:" << titleReader.errorString();
            break;
        }
    }

    titleFile.close();

    return true;
}

bool AbstractBookReader::getQuranPage(QuaZip *zip, LibraryBook *book, BookPage *page)
{
    Q_UNUSED(book);

    // Get the page
    QuaZipFile pagesFile(zip);
    if(zip->setCurrentFile("pages.xml")) {
        if(!pagesFile.open(QIODevice::ReadOnly)) {
            qWarning("getQuranPage: open error %d", pagesFile.getZipError());
            return false;
        }
    }

    QString pid = QString::number(page->pageID);

    QXmlStreamReader bookReader(&pagesFile);
    while(!bookReader.atEnd()) {
        bookReader.readNext();

        if(bookReader.tokenType() == QXmlStreamReader::StartElement) {
            if(bookReader.name() == "item") {
                if(pid == bookReader.attributes().value("id")) {
                    page->part = bookReader.attributes().value("part").toString().toInt();
                    page->page = bookReader.attributes().value("page").toString().toInt();
                    page->sora = bookReader.attributes().value("sora").toString().toInt();
                    page->aya = bookReader.attributes().value("aya").toString().toInt();

                    QuranSora *quranSora = MW->readerHelper()->getQuranSora(page->sora);
                    if(quranSora)
                        page->title = tr("سورة %1، الاية %2").arg(quranSora->name).arg(page->aya);

                    if(bookReader.readNext() == QXmlStreamReader::Characters)
                        page->text = bookReader.text().toString();

                    break;
                }
            }
        }

        if(bookReader.hasError()) {
            qWarning() << "getQuranPage: bookReader error:" << bookReader.errorString();
            break;
        }
    }

    pagesFile.close();

    return true;
}

QString AbstractBookReader::getFileContent(QuaZip *zip, QString fileName)
{
    QuaZipFile file(zip);

    if(zip->setCurrentFile(fileName)) {
        if(file.open(QIODevice::ReadOnly)) {
            return QString::fromUtf8(file.readAll());
        } else {
            qWarning("getFileContent: open error %d", file.getZipError());
        }
    } else {
        qWarning("getFileContent: setCurrentFile error %d", zip->getZipError());
    }

    return QString();
}

void AbstractBookReader::getBookInfo()
{
   QuaZipFile pagesFile(&m_zip);

   if(m_zip.setCurrentFile("pages.xml")) {
       if(!pagesFile.open(QIODevice::ReadOnly)) {
           qWarning("getBookInfo: open error %d", pagesFile.getZipError());
           return;
       }
   }

   m_bookDoc = Utils::getDomDocument(&pagesFile);
   m_rootElement = m_bookDoc.documentElement();
}
