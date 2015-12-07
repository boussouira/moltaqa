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
    void writePage(QList<BookPage*> pages);

    void writeImages();

protected:
    friend class EPubImageReader;

    ZipWriterManager m_zipWriter;
    QStringList m_page;
    QString m_bookUID;
    int m_titleCount;
    int m_lastPageId;
    QHash<int, int> m_sowarPages; ///< Key: Sora number, Value: page number
    QHash<int, int> m_containerPages; ///< Key: Orignal page number, Value: Container page
    QHash<QString, QString> m_images;
    bool m_bookHasImages;
};

#endif // EPUBBOOKEXPORTER_H
