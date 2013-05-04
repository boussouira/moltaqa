#ifndef BOOKINDEXERQURAN_H
#define BOOKINDEXERQURAN_H

#include "bookindexerbase.h"
#include "quazip.h"
#include "quazipfile.h"

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

#endif // BOOKINDEXERQURAN_H
