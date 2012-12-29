#ifndef NEWBOOKWRITER_H
#define NEWBOOKWRITER_H

#include <qhash.h>
#include <qdatetime.h>
#include <qfile.h>
#include <qdom.h>
#include <qxmlstream.h>

#include "ziphelper.h"
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

    ZipHelper m_zipHelper;

    QString m_pagesPath;
    QFile m_pagesFile;
    QXmlStreamWriter m_pagesWriter;

    QString m_titlesPath;
    QFile m_titlesFile;
    QXmlStreamWriter m_titlesWriter;

    int m_lastLavel;
    QHash<int, int> m_levels;
};

#endif // NEWBOOKWRITER_H
