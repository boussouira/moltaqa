#ifndef QURANBOOKINDEXER_H
#define QURANBOOKINDEXER_H

#include "textbookindexer.h"

class QuranBookIndexer : public TextBookIndexer
{
public:
    QuranBookIndexer();

protected:
    void indexPage(BookPage *page);
};

#endif // QURANBOOKINDEXER_H
