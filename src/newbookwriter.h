#ifndef NEWBOOKWRITER_H
#define NEWBOOKWRITER_H

#include <qhash.h>
#include <qdatetime.h>
#include <qfile.h>
#include <qdom.h>

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

#include "sqlutils.h"

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
    int addPage(const QString &text, int pageID, int pageNum, int partNum,
                int hadditNum=0, int ayaNum=0, int soraNum=0);
    void addTitle(const QString &title, int tid, int level);

protected:
    QString processPageText(QString text);

protected:
    QString m_booksDir;
    QString m_bookPath;

    QuaZip m_zip;
    QDomDocument m_pagesDoc;
    QDomElement m_pagesElemnent;
    QDomDocument m_titlesDoc;
    QDomElement m_titlesElement;
    QDomElement m_lastTitlesElement;

    QHash<int, QDomNode> m_levels;
};

#endif // NEWBOOKWRITER_H
