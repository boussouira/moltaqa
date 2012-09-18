#ifndef EPUBBOOKEXPORTER_H
#define EPUBBOOKEXPORTER_H

#include "bookexporter.h"
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"

class EPubBookExporter : public BookExporter
{
    Q_OBJECT
public:
    EPubBookExporter(QObject *parent = 0);

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

    void write(const QString &fileName, const QString &data);
    void writePage(BookPage *page);

protected:
    QuaZip m_zip;
    QStringList m_page;
    QString m_bookUID;
    int m_titleCount;
    QHash<int, int> m_sowarPages; ///< Key: Sora number, Value: page number
};

#endif // EPUBBOOKEXPORTER_H
