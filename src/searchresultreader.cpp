#include "searchresultreader.h"
#include "abstractbookreader.h"
#include "bookreaderhelper.h"
#include "mainwindow.h"
#include "utils.h"
#include <qfile.h>
#include <quazip.h>
#include <qxmlstream.h>

SearchResultReader::SearchResultReader(QObject *parent) :
    QObject(parent)
{
    m_readerHelper = MW->readerHelper();
    m_pagesDom.setMaxCost(10);
    m_titlesDom.setMaxCost(10);
    m_showPageTitle = true;
    m_showPageInfo = true;
    m_hierarchyTitle = Utils::Settings::get("Search/hierarchyTitle", true).toBool();
}

void SearchResultReader::setShowPageTitle(bool show)
{
    m_showPageTitle = show;
}

void SearchResultReader::setShowPageInfo(bool show)
{
    m_showPageInfo = show;
}

bool SearchResultReader::getBookPage(LibraryBookPtr book, BookPage *page)
{
    if(!book) {
        qWarning("SearchResultReader::getBookPage No book with given id");
        return false;
    }

    if(!QFile::exists(book->path)) {
        qWarning() << "SearchResultReader::getBookPage File doesn't exists:" << book->path;
        return false;
    }

    QFile zipFile(book->path);
    QuaZip zip(&zipFile);

    if(!zip.open(QuaZip::mdUnzip)) {
        qWarning() << "SearchResultReader::getBookPage Can't open zip file"
                   << book->path << "error:" << zip.getZipError();
    }

    if(book->isNormal() || book->isTafessir())
        return getSimpleBookPage(&zip, book, page);
    else if(book->isQuran())
        return getQuranPage(&zip, book, page);
    else
        qWarning("SearchResultReader::getBookPage Unknow book type");

    zip.close();

    return false;
}

bool SearchResultReader::getSimpleBookPage(QuaZip *zip, LibraryBookPtr book, BookPage *page)
{
    // Page info
    if(m_showPageInfo) {
        ml_return_val_on_fail(getPageInfo(zip, book, page), false);
    }

    // Page text
    ml_return_val_on_fail(getPageText(zip, page), false);

    // Page title
    if(m_showPageTitle) {
        getPageTitle(zip, book, page);
    }

    return true;
}

bool SearchResultReader::getQuranPage(QuaZip *zip, LibraryBookPtr book, BookPage *page)
{
    Q_UNUSED(book);

    // Get the page
    QuaZipFile pagesFile(zip);
    if(zip->setCurrentFile("pages.xml")) {
        if(!pagesFile.open(QIODevice::ReadOnly)) {
            qWarning() << "getQuranPage: open book pages error" << pagesFile.getZipError()
                       << "book" << book->title
                       << "id" << book->id;
            return false;
        }
    }

    QString pid = QString::number(page->pageID);

    QXmlStreamReader bookReader(&pagesFile);
    while(!bookReader.atEnd()) {
        bookReader.readNext();

        if(bookReader.isStartElement()) {
            if(bookReader.name() == "item") {
                if(pid == bookReader.attributes().value("id")) {
                    page->part = bookReader.attributes().value("part").toString().toInt();
                    page->part = qMax(1, page->part);

                    page->page = bookReader.attributes().value("page").toString().toInt();
                    page->sora = bookReader.attributes().value("sora").toString().toInt();
                    page->aya = bookReader.attributes().value("aya").toString().toInt();

                    if(page->sora) {
                        QuranSora *quranSora = m_readerHelper->getQuranSora(page->sora);
                        if(quranSora)
                            page->title = tr("سورة %1، الاية %2").arg(quranSora->name).arg(page->aya);
                    }

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

bool SearchResultReader::getPageInfo(QuaZip *zip, LibraryBookPtr book, BookPage *page)
{
    QTime time;
    time.start();

    QuaZipFile pagesFile(zip);
    XmlDomHelper *pagesDom = 0;

    if(m_pagesDom.contains(book->id)) {
        pagesDom = m_pagesDom.object(book->id);

        if(!pagesDom) {
            m_pagesDom.remove(book->id);
            qWarning("SearchResultReader: Null saved pages DOM");
            return false;
        }
    } else {
        if(zip->setCurrentFile("pages.xml")) {
            if(!pagesFile.open(QIODevice::ReadOnly)) {
                qWarning() << "getSimpleBookPage: open book pages error" << pagesFile.getZipError()
                           << "book" << book->title
                           << "id" << book->id;

                return false;
            }
        }

        pagesDom = new XmlDomHelper;
        pagesDom->load(&pagesFile);
    }

    QDomElement pageElement = pagesDom->findElement("id", page->pageID);
    if(!pageElement.isNull()) {
        page->part = pageElement.attribute("part").toInt();
        page->page = pageElement.attribute("page").toInt();

        if(book->isTafessir()) {
            page->sora = pageElement.attribute("sora").toInt();
            page->aya = pageElement.attribute("aya").toInt();
        }
    } else {
        return false;
    }

    if(pagesFile.isOpen())
        pagesFile.close();

    if(!m_pagesDom.contains(book->id)) {
        if(time.elapsed() > 1000)
            m_pagesDom.insert(book->id, pagesDom);
        else
            delete pagesDom;
    }

    return true;
}

bool SearchResultReader::getPageText(QuaZip *zip, BookPage *page)
{
    page->text = AbstractBookReader::getPageText(zip, page->pageID);

    return true;
}

bool SearchResultReader::getPageTitle(QuaZip *zip, LibraryBookPtr book, BookPage *page)
{
    // If we have a saved model we use it to get the title
    QString title = m_readerHelper->getTitleText(book->id,
                                                 page->titleID,
                                                 m_hierarchyTitle && !book->isTafessir());
    if(title.size()) {
        page->title = title;

        if(book->isTafessir())
            taffesirTitle(page);

        return true;
    }

    QuaZipFile titleFile(zip);
    XmlDomHelper *titlesDom = 0;

    if(m_titlesDom.contains(book->id)) {
        titlesDom = m_titlesDom.object(book->id);

        if(!titlesDom) {
            m_titlesDom.remove(book->id);
            qWarning("SearchResultReader: Null saved titles DOM");
            return false;
        }
    } else {
        if(zip->setCurrentFile("titles.xml")) {
            if(!titleFile.open(QIODevice::ReadOnly)) {
                qWarning() << "getSimpleBookPage: open book titles error" << titleFile.getZipError()
                           << "book" << book->title
                           << "id" << book->id;

                return false;
            }
        }

        titlesDom = new XmlDomHelper;
        titlesDom->load(&titleFile);
    }

    QDomElement titleElement = titlesDom->treeFindElement("pageID", page->titleID);
    if(!titleElement.isNull()) {
        if(book->isTafessir()) {
            page->title = titleElement.firstChildElement("text").text();
            taffesirTitle(page);
        } else {
            QStringList list;
            do {
                list << titleElement.firstChildElement("text").text();

                titleElement = titleElement.parentNode().toElement();
            } while (m_hierarchyTitle
                     && !titleElement.isNull()
                     && titleElement != titlesDom->rootElement());

            page->title = BookReaderHelper::formatTitlesList(list);;
        }
    } else {
        return false;
    }

    if(titleFile.isOpen())
        titleFile.close();

    if(!m_titlesDom.contains(book->id)) {
        if(m_pagesDom.contains(book->id))
            m_titlesDom.insert(book->id, titlesDom);
        else
            delete titlesDom;
    }

    return true;
}

void SearchResultReader::taffesirTitle(BookPage *page)
{
    if(page->sora && page->title.size() < 9) {
        QuranSora *quranSora = m_readerHelper->getQuranSora(page->sora);
        if(quranSora)
            page->title = tr("تفسير سورة %1، الاية %2").arg(quranSora->name).arg(page->aya);
    }
}
