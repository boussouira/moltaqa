#ifndef EPUBBOOKEXPORTER_H
#define EPUBBOOKEXPORTER_H

#include "bookexporter.h"
#include "ziphelper.h"

class EPubBookExporter : public BookExporter
{
    Q_OBJECT
public:
    EPubBookExporter(QObject *parent = 0);

    bool multiBookExport() { return false; }
    bool moveGeneratedFile() { return true; }
    void start();

protected:
    void openZip();
    void closeZip();

    void init();
    void writePages();
    void writeBookInfo();
    void writeAuthorInfo();
    void writeIntro();
    void writeContent();
    void writeTOC();

    void writeSimpleBookTOC(QTextStream &out);
    void writeQuranBookTOC(QTextStream &out);
    void writeTocItem(class QDomElement &element, QTextStream &out);

    void write(const QString &fileName, const QString &data, bool prepend);
    void writePage(BookPage *page);

protected:
    ZipWriterManager m_zipWriter;
    QStringList m_page;
    QString m_bookUID;
    int m_titleCount;
    QHash<int, int> m_sowarPages; ///< Key: Sora number, Value: page number
};

#endif // EPUBBOOKEXPORTER_H
