#ifndef TEXTBOOKINDEXER_H
#define TEXTBOOKINDEXER_H

#include "librarybook.h"

namespace lucene {
    namespace index { class IndexWriter; }
    namespace document { class Document; }
}

class BookIndexerBase
{
public:
    BookIndexerBase();
    virtual ~BookIndexerBase();

    void setIndexWriter(lucene::index::IndexWriter *writer);
    void setLibraryBook(LibraryBook::Ptr book);

    virtual void open()=0;
    virtual void start()=0;

protected:
    void addPageToIndex(BookPage *page);
    void indexPage(BookPage *page);

    virtual void morePageIndex(BookPage *page);

protected:
    lucene::index::IndexWriter *m_writer;
    lucene::document::Document *m_doc;
    LibraryBook::Ptr m_book;
    wchar_t *m_bookIdW;
    int m_tokenAndNoStore;
    int m_storeAndNoToken;
};

#endif // TEXTBOOKINDEXER_H
