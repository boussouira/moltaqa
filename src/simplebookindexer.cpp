#include "simplebookindexer.h"
#include "clheader.h"
#include "clutils.h"
#include "clconstants.h"
#include "librarymanager.h"
#include "authorsmanager.h"
#include <qdebug.h>

SimpleBookIndexer::SimpleBookIndexer() :
    m_authorDeath(0),
    m_unknowAuthor(false)
{
}

SimpleBookIndexer::~SimpleBookIndexer()
{
    if(m_authorDeath)
        free(m_authorDeath);
}

void SimpleBookIndexer::indexPage(BookPage *page)
{
    if(!m_authorDeath && !m_unknowAuthor) {
        AuthorInfoPtr author = LibraryManager::instance()->authorsManager()->getAuthorInfo(m_book->authorID);
        if(author) {
            int death = (author->isALive ? 9999 : (author->unknowDeath ? 0 : author->deathYear));
            m_authorDeath = Utils::CLucene::intToWChar(death);
        } else {
            m_unknowAuthor = true;
        }
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
