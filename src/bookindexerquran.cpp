#include "bookindexerquran.h"
#include "clheader.h"
#include "clutils.h"
#include "utils.h"
#include "clconstants.h"
#include "bookexception.h"

#include <qfile.h>
#include <qxmlstream.h>

BookIndexerQuran::BookIndexerQuran()
{
}

void BookIndexerQuran::open()
{
    if(!m_book)
        throw BookException("BookIndexerQuran::open book is null");

    if(!QFile::exists(m_book->path))
               throw BookException("BookIndexerQuran::open file doesn't exists", m_book->path);

    m_zip.setZipName(m_book->path);

    if(!m_zip.open(QuaZip::mdUnzip))
        throw BookException("BookIndexerQuran::open Can't zip file",
                                m_book->path, m_zip.getZipError());
}

void BookIndexerQuran::start()
{
    ml_return_on_fail2(m_bookIdW, "BookIndexerSimple::start m_bookIdW is null");

    // Get the page
    QuaZipFile pagesFile(&m_zip);
    if(m_zip.setCurrentFile("pages.xml")) {
        if(!pagesFile.open(QIODevice::ReadOnly)) {
            qWarning() << "BookIndexerQuran::start open book pages error" << pagesFile.getZipError()
                       << "book" << m_book->title
                       << "id" << m_book->id;
            return;
        }
    }

    BookPage page;

    QXmlStreamReader bookReader(&pagesFile);
    while(!bookReader.atEnd()) {
        bookReader.readNext();

        if(bookReader.isStartElement()) {
            if(bookReader.name() == "aya") {
                page.pageID = bookReader.attributes().value("id").toString().toInt();
                page.part = bookReader.attributes().value("part").toString().toInt();
                page.part = qMax(1, page.part);

                page.page = bookReader.attributes().value("page").toString().toInt();
                page.sora = bookReader.attributes().value("sora").toString().toInt();
                page.aya = bookReader.attributes().value("aya").toString().toInt();

                if(bookReader.readNext() == QXmlStreamReader::Characters)
                    page.text = bookReader.text().toString();

                addPageToIndex(&page);
            }
        }
    }

    if(bookReader.hasError()) {
        throw BookException("BookIndexerQuran::start bookReader error",
                            bookReader.errorString());
    }

    pagesFile.close();
}

void BookIndexerQuran::morePageIndex(BookPage *page)
{
    if(page->sora) {
        m_doc->add( *_CLNEW Field(QURAN_SORA_FIELD,
                                  Utils::CLucene::intToWChar(page->sora),
                                  m_storeAndNoToken, false));
    }
}
