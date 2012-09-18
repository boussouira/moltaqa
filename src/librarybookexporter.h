#ifndef LIBRARYBOOKEXPORTER_H
#define LIBRARYBOOKEXPORTER_H

#include "bookexporter.h"
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"

class LibraryBookExporter : public BookExporter
{
    Q_OBJECT
public:
    LibraryBookExporter(QObject *parent = 0);

    void start();

protected:
    void openZip();
    void closeZip();

    void addBookInfo();
    void addAuthorInfo();
    void addBookFile();

    void deleteTemp();

protected:
    QuaZip m_zip;
    QStringList m_tempFiles;
};

#endif // LIBRARYBOOKEXPORTER_H
