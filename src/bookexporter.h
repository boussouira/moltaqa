#ifndef BOOKEXPORTER_H
#define BOOKEXPORTER_H

#include <qobject.h>
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

#include "librarybook.h"

class BookExporter : public QObject
{
public:
    BookExporter();

    void setLibraryBook(LibraryBookPtr book) { m_book = book; }
    QString genereatedPath() { return m_genereatedPath; }

    virtual void start();

protected:
    void openZip();
    void closeZip();

    void addBookInfo();
    void addAuthorInfo();
    void addBookFile();

    void deleteTemp();

protected:
    LibraryBookPtr m_book;
    QuaZip m_zip;
    QString m_genereatedPath;
    QString m_tempDir;
    QStringList m_tempFiles;
};

#endif // BOOKEXPORTER_H
