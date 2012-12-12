#ifndef LIBRARYBOOKEXPORTER_H
#define LIBRARYBOOKEXPORTER_H

#include "bookexporter.h"
#include "xmldomhelper.h"
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"

class LibraryBookExporter : public BookExporter
{
    Q_OBJECT
public:
    LibraryBookExporter(QObject *parent = 0);

    void start();

protected:
    void createZip();
    void closeZip();

    void addBookInfo();
    void addAuthorInfo();
    void addBookFile();

    void deleteTemp();

protected:
    QuaZip m_zip;
    QStringList m_tempFiles;
    XmlDomHelper m_contentDom;
    QDomElement m_booksElement;
    QDomElement m_authorsElement;
};

#endif // LIBRARYBOOKEXPORTER_H
