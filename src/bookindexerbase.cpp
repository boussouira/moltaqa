#include "bookindexerbase.h"
#include "clheader.h"
#include "clutils.h"
#include "clconstants.h"
#include "bookexception.h"
#include "stringutils.h"
#include "utils.h"

BookIndexerBase::BookIndexerBase() :
    m_writer(0),
    m_doc(new Document()),
    m_book(0),
    m_bookIdW(0)
{
    m_tokenAndNoStore = Field::STORE_NO | Field::INDEX_TOKENIZED;
    m_storeAndNoToken = Field::STORE_YES | Field::INDEX_UNTOKENIZED;
}

BookIndexerBase::~BookIndexerBase()
{
    ml_delete_check(m_doc);

    if(m_bookIdW)
        free(m_bookIdW);
}

void BookIndexerBase::setIndexWriter(IndexWriter *writer)
{
    m_writer = writer;
}

void BookIndexerBase::setLibraryBook(LibraryBook::Ptr book)
{
    m_book = book;
    m_bookIdW = Utils::CLucene::intToWChar(m_book->id);
}

void BookIndexerBase::addPageToIndex(BookPage *page)
{
    indexPage(page);
    morePageIndex(page);

    m_writer->addDocument(m_doc);

    m_doc->clear();
    page->clear();
}

void BookIndexerBase::indexPage(BookPage *page)
{
    m_doc->add( *_CLNEW Field(PAGE_ID_FIELD,
                              Utils::CLucene::intToWChar(page->pageID),
                              m_storeAndNoToken, false));

    m_doc->add( *_CLNEW Field(BOOK_ID_FIELD,
                              m_bookIdW, m_storeAndNoToken));

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

void BookIndexerBase::morePageIndex(BookPage *page)
{
    Q_UNUSED(page);
}
