#include "textbookindexer.h"
#include "clheader.h"
#include "clutils.h"
#include "clconstants.h"
#include "bookexception.h"
#include "textquranreader.h"
#include "textsimplebookreader.h"
#include "texttafessirreader.h"
#include "stringutils.h"

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

    m_doc = new Document();

    BookPage *page = m_reader->page();
    bookW = Utils::intToWChar(m_book->bookID);

    while (m_reader->hasNext()) {
        m_reader->nextPage();

        if(page->text.isEmpty())
            continue;

        pageW = Utils::intToWChar(page->pageID);

        m_doc->add( *_CLNEW Field(BOOK_ID_FIELD, bookW, m_storeAndNoToken));
        m_doc->add( *_CLNEW Field(PAGE_ID_FIELD, pageW, m_storeAndNoToken, false));

        indexPageText(page);
        indexPage(page);

        m_writer->addDocument(m_doc);
        m_doc->clear();
    }

    delete m_doc;
}

void TextBookIndexer::indexPageText(BookPage *page)
{
    QString pageText = Utils::removeHtmlSpecialChars(page->text);

    wchar_t *fullText = Utils::QStringToWChar(Utils::removeHtmlTags(pageText));
    m_doc->add( *_CLNEW Field(PAGE_TEXT_FIELD,
                              fullText,
                              m_tokenAndNoStore, false));

    QString asanid = Utils::getTagsText(pageText, "sanad");
    if(!asanid.isEmpty())
        m_doc->add( *_CLNEW Field(HADDIT_SANAD_FIELD,
                                  Utils::QStringToWChar(asanid),
                                  m_tokenAndNoStore, false));

    QString motoon = Utils::getTagsText(pageText, "mateen");
    if(!motoon.isEmpty())
        m_doc->add( *_CLNEW Field(HADDIT_MATEEN_FIELD,
                                  Utils::QStringToWChar(motoon),
                                  m_tokenAndNoStore, false));

    QString sheer = Utils::getTagsText(pageText, "sheer");
    if(!sheer.isEmpty())
        m_doc->add( *_CLNEW Field(SHEER_FIELD,
                                  Utils::QStringToWChar(sheer),
                                  m_tokenAndNoStore, false));
}
