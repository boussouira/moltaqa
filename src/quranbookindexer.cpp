#include "quranbookindexer.h"
#include "clheader.h"
#include "clutils.h"
#include "clconstants.h"

QuranBookIndexer::QuranBookIndexer()
{
}

void QuranBookIndexer::indexPage(BookPage *page)
{
    m_doc->add( *_CLNEW Field(QURAN_SORA_FIELD,
                              Utils::CLucene::intToWChar(page->sora),
                              m_storeAndNoToken, false));
}
