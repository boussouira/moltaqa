#ifndef BOOKFILESREADER_H
#define BOOKFILESREADER_H

#include "librarybook.h"
#include "quazip.h"

class BookFilesReader
{
public:
    BookFilesReader();

    LibraryBook::Ptr book() const;
    void setBook(const LibraryBook::Ptr &book);

    void start();

protected:
    void open();
    void close();

    virtual bool acceptFile(QString filePath);

    virtual bool readFile(QString filePath, QIODevice &file);
    virtual void readPageText(int pageID, QString filePath, QString text);

protected:
    LibraryBook::Ptr m_book;
    QuaZip m_zip;
};

#endif // BOOKFILESREADER_H
