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
    void createNewBook(QString bookPath=QString());
    QString bookPath();

    /// Begins a transaction on the database
    void startReading();

    /// Commits a transaction to the database
    void endReading();
    int addPage(const QString &text, int pageID, int pageNum, int partNum,
                int hadditNum=-1, int ayaNum=-1, int soraNum=-1);
    void addTitle(const QString &title, int tid, int level);

protected:
    QString m_tempFolder;
    QString m_bookPath;

    QuaZip m_zip;
    QDomDocument m_pagesDoc;
    QDomElement m_pagesElemnent;
    QDomDocument m_titlesDoc;
    QDomElement m_titlesElement;
    QDomElement m_lastTitlesElement;

    int m_pageId;
    int m_prevID;
    int m_lastLevel;
    int m_titleID;

    QHash<int, int> m_idsHash; ///< For mapping between shamela ids and our ids
    QTime m_time;
};

#endif // NEWBOOKWRITER_H
