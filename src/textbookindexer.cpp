#include "textbookindexer.h"
#include "clheader.h"
#include "clutils.h"
#include "clconstants.h"
#include "bookexception.h"
#include "textquranreader.h"
#include "textsimplebookreader.h"
#include "texttafessirreader.h"
#include "stringutils.h"
#include "utils.h"

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
    ml_delete_check(m_reader);
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
        throw BookException("Unknow book type", QString("Type: %1").arg(m_book->type));

    m_reader->setBookInfo(m_book);
    m_reader->openBook();
    m_reader->load();
}

void TextBookIndexer::start()
{
    ml_return_on_fail2(m_reader->pagesCount(), "BookIndexer: Book" << m_book->title << "is empty");

    wchar_t *bookW;
    wchar_t *pageW;

    m_doc = new Document();

    BookPage *page = m_reader->page();
    bookW = Utils::CLucene::intToWChar(m_book->id);

    while (m_reader->hasNext()) {
        m_reader->nextPage();

        if(page->text.isEmpty())
            continue;

        pageW = Utils::CLucene::intToWChar(page->pageID);

        m_doc->add( *_CLNEW Field(BOOK_ID_FIELD, bookW, m_storeAndNoToken));
        m_doc->add( *_CLNEW Field(PAGE_ID_FIELD, pageW, m_storeAndNoToken, false));

        indexPageText(page);
        indexPage(page);

        m_writer->addDocument(m_doc);
        m_doc->clear();
    }

    free(bookW);
    delete m_doc;
}

void TextBookIndexer::indexPageText(BookPage *page)
{
    QString pageText = Utils::Html::removeSpecialChars(page->text);

    wchar_t *fullText = Utils::CLucene::QStringToWChar(Utils::Html::removeTags(pageText));
    m_doc->add( *_CLNEW Field(PAGE_TEXT_FIELD,
                              fullText,
                              m_tokenAndNoStore, false));

    QString asanid = Utils::Html::getTagsText(pageText, "sanad");
    if(asanid.size())
        m_doc->add( *_CLNEW Field(HADDIT_SANAD_FIELD,
                                  Utils::CLucene::QStringToWChar(asanid),
                                  m_tokenAndNoStore, false));

    QString motoon = Utils::Html::getTagsText(pageText, "mateen");
    if(motoon.size())
        m_doc->add( *_CLNEW Field(HADDIT_MATEEN_FIELD,
                                  Utils::CLucene::QStringToWChar(motoon),
                                  m_tokenAndNoStore, false));

    QString sheer = Utils::Html::getTagsText(pageText, "sheer");
    if(sheer.size())
        m_doc->add( *_CLNEW Field(SHEER_FIELD,
                                  Utils::CLucene::QStringToWChar(sheer),
                                  m_tokenAndNoStore, false));
}
