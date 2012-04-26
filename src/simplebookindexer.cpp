#include "simplebookindexer.h"
#include "clheader.h"
#include "clutils.h"
#include "clconstants.h"
#include "librarymanager.h"
#include "authorsmanager.h"
#include <qdebug.h>

SimpleBookIndexer::SimpleBookIndexer() :
    m_authorDeath(0)
{
}

SimpleBookIndexer::~SimpleBookIndexer()
{
    if(m_authorDeath)
        free(m_authorDeath);
}

void SimpleBookIndexer::indexPage(BookPage *page)
{
    if(!m_authorDeath) {
        AuthorInfoPtr author = LibraryManager::instance()->authorsManager()->getAuthorInfo(m_book->authorID);
        if(author)
            m_authorDeath = Utils::CLucene::intToWChar(author->deathYear);
    }

    m_doc->add( *_CLNEW Field(TITLE_ID_FIELD,
                              Utils::CLucene::intToWChar(page->titleID),
                              m_storeAndNoToken, false));

    if(m_authorDeath) {
        m_doc->add( *_CLNEW Field(AUTHOR_DEATH_FIELD,
                                  m_authorDeath,
                                  m_storeAndNoToken));
    }

    if(page->title.size()) {
        m_doc->add( *_CLNEW Field(TITLE_TEXT_FIELD,
                                  Utils::CLucene::QStringToWChar(page->title),
                                  m_tokenAndNoStore, false));
    }
}
