#ifndef QURANBOOKINDEXER_H
#define QURANBOOKINDEXER_H

#include "textbookindexer.h"
#include <quazip.h>
#include <quazipfile.h>

class BookIndexerQuran : public BookIndexerBase
{
public:
    BookIndexerQuran();

    void open();
    void start();

protected:
    void morePageIndex(BookPage *page);

protected:
    QuaZip m_zip;
};

#endif // QURANBOOKINDEXER_H
