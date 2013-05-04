#include "searchresultreader.h"
#include "abstractbookreader.h"
#include "bookreaderhelper.h"
#include "mainwindow.h"
#include "quazip.h"
#include "utils.h"

#include <qfile.h>
#include <qstack.h>
#include <qxmlstream.h>

SearchResultReader::SearchResultReader(QObject *parent) :
    QObject(parent)
{
    m_readerHelper = MW->readerHelper();
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

bool SearchResultReader::getBookPage(LibraryBook::Ptr book, BookPage *page)
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

bool SearchResultReader::getSimpleBookPage(QuaZip *zip, LibraryBook::Ptr book, BookPage *page)
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

bool SearchResultReader::getQuranPage(QuaZip *zip, LibraryBook::Ptr book, BookPage *page)
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
            if(bookReader.name() == "aya") {
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

bool SearchResultReader::getPageInfo(QuaZip *zip, LibraryBook::Ptr book, BookPage *page)
{
    QuaZipFile pagesFile(zip);
    if(zip->setCurrentFile("pages.xml")) {
        if(!pagesFile.open(QIODevice::ReadOnly)) {
            qWarning() << "getSimpleBookPage: open book pages error" << pagesFile.getZipError()
                       << "book" << book->title
                       << "id" << book->id;

            return false;
        }
    }

    QXmlStreamReader reader(&pagesFile);
    QString pageIdStr = QString::number(page->pageID);

    while(!reader.atEnd()) {
        reader.readNext();

        if(reader.isStartElement()) {
            if(reader.name() == "page") {
                if(reader.attributes().value("id") == pageIdStr) {
                    page->part = reader.attributes().value("part").toString().toInt();
                    page->page = reader.attributes().value("page").toString().toInt();
                    if(book->isTafessir()) {
                        page->sora = reader.attributes().value("sora").toString().toInt();
                        page->aya = reader.attributes().value("aya").toString().toInt();
                    }

                    return true;
                }
            }
        }

    }

    if(reader.hasError()) {
        qDebug() << "getTitles: QXmlStreamReader error:" << reader.errorString();
    }

    return false;
}

bool SearchResultReader::getPageText(QuaZip *zip, BookPage *page)
{
    page->text = AbstractBookReader::getPageText(zip, page->pageID);

    return true;
}

bool SearchResultReader::getPageTitle(QuaZip *zip, LibraryBook::Ptr book, BookPage *page)
{
    QuaZipFile titleFile(zip);

    if(zip->setCurrentFile("titles.xml")) {
        if(!titleFile.open(QIODevice::ReadOnly)) {
            qWarning() << "getSimpleBookPage: open book titles error" << titleFile.getZipError()
                       << "book" << book->title
                       << "id" << book->id;

            return false;
        }
    }

    QXmlStreamReader reader(&titleFile);
    QString titleIdStr = QString::number(page->titleID);
    QStack<QString> titles;

    while(!reader.atEnd()) {
        reader.readNext();

        if(reader.isStartElement()) {
            if(reader.name() == "title") {
                if(reader.attributes().value("pageID") == titleIdStr) {
                    if(reader.readNext() == QXmlStreamReader::Characters
                            && reader.readNext() == QXmlStreamReader::StartElement
                            && reader.name() == "text") {
                        if(reader.readNext() == QXmlStreamReader::Characters) {
                            page->title = reader.text().toString();
                            if(book->isTafessir())
                                taffesirTitle(page);

                            if(m_hierarchyTitle) {
                                titles.push(page->title);
                                page->title = BookReaderHelper::formatTitlesList(titles);
                            }
                            return true;
                        } else {
                            if(reader.tokenType() != QXmlStreamReader::EndElement) { // Ignore empty titles
                                qWarning() << "SearchResultReader::getPageTitle Unexpected token type"
                                           << reader.tokenString() << "- Book:" << book->id
                                           << book->title << book->fileName;
                            }

                            break;
                        }
                    } else {
                        break;
                    }
                }
            } else if(m_hierarchyTitle && reader.name() == "text") {
                if(reader.readNext() == QXmlStreamReader::Characters) {
                    titles.push(reader.text().toString());
                }
            }
        } else if(m_hierarchyTitle && reader.isEndElement()) {
            if(reader.name() == "title" && !titles.isEmpty()) {
                titles.pop();
            }
        }
    }

    if(reader.hasError()) {
        qDebug() << "SearchResultReader::getPageTitle QXmlStreamReader error:" << reader.errorString()
                 << "- Book:" << book->id << book->title << book->fileName;
    }

    return false;
}

void SearchResultReader::taffesirTitle(BookPage *page)
{
    if(page->sora && page->title.size() < 9) {
        QuranSora *quranSora = m_readerHelper->getQuranSora(page->sora);
        if(quranSora)
            page->title = tr("تفسير سورة %1، الاية %2").arg(quranSora->name).arg(page->aya);
    }
}
