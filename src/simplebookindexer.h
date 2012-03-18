#ifndef SIMPLEBOOKINDEXER_H
#define SIMPLEBOOKINDEXER_H

#include "textbookindexer.h"

class SimpleBookIndexer : public TextBookIndexer
{
public:
    SimpleBookIndexer();

protected:
    void indexPage(BookPage *page);
};

#endif // SIMPLEBOOKINDEXER_H
