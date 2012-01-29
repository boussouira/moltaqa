#ifndef NEWQURANWRITER_H
#define NEWQURANWRITER_H

#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsqlerror.h>
#include <qhash.h>
#include <qapplication.h>

#include <qfile.h>
#include <qdom.h>

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

class newQuranWriter
{
public:
    newQuranWriter();
    void createNewBook(QString bookPath=QString());
    QString bookPath();

    /// Begins a transaction on the database
    void startReading();

    /// Commits a transaction to the database
    void endReading();
    void addPage(const QString &text, int soraNum, int ayaNum, int pageNum);

protected:
    QString m_tempFolder;
    QString m_bookPath;
    QuaZip m_zip;
    QDomDocument m_pagesDoc;
    QDomElement m_pagesElemnent;
    int m_pageId;
};

#endif // NEWQURANWRITER_H
