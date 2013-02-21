#ifndef NEWBOOKWRITER_H
#define NEWBOOKWRITER_H

#include <qhash.h>
#include <qdatetime.h>
#include <qfile.h>
#include <qdom.h>
#include <qxmlstream.h>

#include "ziphelper.h"
#include "sqlutils.h"

struct BookTitle
{
    int pageID;
    int level;
    QString title;
    QString tid;
};

class NewBookWriter
{
public:
    NewBookWriter();
    ~NewBookWriter();

    void createNewBook();
    QString bookPath();

    /// Prepare zip and xml files
    void startReading();

    /// Close the zip file
    void endReading();

    /// Write titles.xml and  pages.xml to the zip file
    void writeMetaFiles();

    void addPage(BookPage *page);
    void addPageText(int pageID, const QString &text);

    void addTitle(const QString &title, int tid, int level);

protected:
    QString processPageText(int pageID, QString text);

protected:
    QString m_booksDir;
    QString m_bookPath;

    ZipWriterManager m_zipWriter;

    QString m_pagesPath;
    QFile m_pagesFile;
    QXmlStreamWriter m_pagesWriter;

    QString m_titlesPath;
    QFile m_titlesFile;
    QXmlStreamWriter m_titlesWriter;
    QHash<int, QList<BookTitle> > m_titles;
    int m_titlesCount;

    int m_lastLavel;
    QHash<int, int> m_levels;

    bool m_writeMeta;
};

#endif // NEWBOOKWRITER_H
