#ifndef LIBRARYBOOKEXPORTER_H
#define LIBRARYBOOKEXPORTER_H

#include "bookexporter.h"
#include "xmldomhelper.h"
#include "quazip.h"
#include "quazipfile.h"

class LibraryBookExporter : public BookExporter
{
    Q_OBJECT
public:
    LibraryBookExporter(QObject *parent = 0);

    bool multiBookExport() { return true; }
    bool moveGeneratedFile() { return true; }
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
    QList<int> m_addedAuthorsInfo;
};

#endif // LIBRARYBOOKEXPORTER_H
