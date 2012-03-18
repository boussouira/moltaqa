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
    ~TextBookIndexer();

    void setIndexWriter(lucene::index::IndexWriter *writer) { m_writer = writer; }
    void setLibraryBook(LibraryBook *book) { m_book = book; }

    void open();
    void start();

protected:
    virtual void indexPage(BookPage *page)=0;

protected:
    lucene::index::IndexWriter *m_writer;
    lucene::document::Document *m_doc;
    LibraryBook *m_book;
    TextBookReader *m_reader;
    int m_tokenAndNoStore;
    int m_storeAndNoToken;
};

#endif // TEXTBOOKINDEXER_H
