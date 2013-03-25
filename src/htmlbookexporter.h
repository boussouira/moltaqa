#ifndef HTMLBOOKEXPORTER_H
#define HTMLBOOKEXPORTER_H

#include "bookexporter.h"
#include <qdom.h>

class QDir;
class RichBookReader;

class HtmlBookExporter : public BookExporter
{
    Q_OBJECT
public:
    HtmlBookExporter(QObject *parent = 0);
    
    bool multiBookExport() { return false; }
    bool moveGeneratedFile() { return false; }
    void start();

protected:
    void openReader();
    void copyStyleTemplate(QString styleDirPath);
    void writeTOC();
    void writeTocItem(QTextStream &out, QDomElement &element);
    void writePages();
    void writePage(QDir &dir, BookPage *page);
    void writeImages();

protected:
    RichBookReader *m_reader;
    QList<int> m_pageTitles;
    QHash<int, int> m_sowarPages; ///< Key: Sora number, Value: page number
    bool m_bookHasImages;
};

#endif // HTMLBOOKEXPORTER_H
