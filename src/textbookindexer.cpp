#include "textbookindexer.h"
#include "clheader.h"
#include "clutils.h"
#include "clconstants.h"
#include "bookexception.h"
#include "textquranreader.h"
#include "textsimplebookreader.h"
#include "texttafessirreader.h"

TextBookIndexer::TextBookIndexer() :
    m_writer(0),
    m_doc(0),
    m_book(0),
    m_reader(0)
{
    m_tokenAndNoStore = Field::STORE_NO | Field::INDEX_TOKENIZED;
    m_storeAndNoToken = Field::STORE_YES | Field::INDEX_UNTOKENIZED;
}

TextBookIndexer::~TextBookIndexer()
{
    if(m_reader)
        delete m_reader;
}

void TextBookIndexer::open()
{
    if(m_book->isNormal())
        m_reader = new TextSimpleBookReader();
    else if (m_book->isTafessir())
        m_reader = new TextTafessirReader();
    else if (m_book->isQuran())
        m_reader = new TextQuranReader();
    else
        throw BookException("Unknow book type", QString("Type: %1").arg(m_book->bookType));

    m_reader->setBookInfo(m_book);
    m_reader->openBook();
    m_reader->load();
}

void TextBookIndexer::start()
{
    wchar_t *bookW;
    wchar_t *pageW;
    wchar_t *textW;

    m_doc = new Document();

    BookPage *page = m_reader->page();
    bookW = Utils::intToWChar(m_book->bookID);

    while (m_reader->hasNext()) {
        m_reader->nextPage();

        if(page->text.isEmpty())
            continue;

        pageW = Utils::intToWChar(page->pageID);
        textW = Utils::QStringToWChar(page->text);

        m_doc->add( *_CLNEW Field(BOOK_ID_FIELD, bookW, m_storeAndNoToken));
        m_doc->add( *_CLNEW Field(PAGE_ID_FIELD, pageW, m_storeAndNoToken, false));
        m_doc->add( *_CLNEW Field(PAGE_TEXT_FIELD, textW, m_tokenAndNoStore, false));

        indexPage(page);

        m_writer->addDocument(m_doc);
        m_doc->clear();
    }

    delete m_doc;
}
