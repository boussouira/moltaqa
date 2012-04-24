#ifndef SIMPLEBOOKINDEXER_H
#define SIMPLEBOOKINDEXER_H

#include "textbookindexer.h"

class SimpleBookIndexer : public TextBookIndexer
{
public:
    SimpleBookIndexer();
    ~SimpleBookIndexer();

protected:
    void indexPage(BookPage *page);

protected:
    wchar_t *m_authorDeath;
};

#endif // SIMPLEBOOKINDEXER_H
