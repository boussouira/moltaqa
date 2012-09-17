#ifndef TEXTBOOKINDEXER_H
#define TEXTBOOKINDEXER_H

#include "librarybook.h"

class TextBookReader;

namespace lucene {
    namespace index { class IndexWriter; }
    namespace document { class Document; }
}

class TextBookIndexer
{
public:
    TextBookIndexer();
    virtual ~TextBookIndexer();

    void setIndexWriter(lucene::index::IndexWriter *writer) { m_writer = writer; }
    void setLibraryBook(LibraryBookPtr book) { m_book = book; }

    void open();
    void start();

protected:
    void indexPageText(BookPage *page);
    virtual void indexPage(BookPage *page)=0;

protected:
    lucene::index::IndexWriter *m_writer;
    lucene::document::Document *m_doc;
    LibraryBookPtr m_book;
    TextBookReader *m_reader;
    int m_tokenAndNoStore;
    int m_storeAndNoToken;
};

#endif // TEXTBOOKINDEXER_H
