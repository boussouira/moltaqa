#include "simplebookindexer.h"
#include "clheader.h"
#include "clutils.h"
#include "clconstants.h"
#include <qdebug.h>

SimpleBookIndexer::SimpleBookIndexer()
{
}

void SimpleBookIndexer::indexPage(BookPage *page)
{
    m_doc->add( *_CLNEW Field(TITLE_ID_FIELD,
                              Utils::intToWChar(page->titleID),
                              m_storeAndNoToken, false));

    if(!page->title.isEmpty()) {
        m_doc->add( *_CLNEW Field(TITLE_TEXT_FIELD,
                                  Utils::QStringToWChar(page->title),
                                  m_tokenAndNoStore, false));
    }
}
