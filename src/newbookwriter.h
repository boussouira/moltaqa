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

    /// Begins a transaction on the database
    void startReading();

    /// Commits a transaction to the database
    void endReading();
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
};

#endif // NEWBOOKWRITER_H
