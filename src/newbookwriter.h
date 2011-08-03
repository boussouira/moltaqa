#ifndef NEWBOOKWRITER_H
#define NEWBOOKWRITER_H

#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsqlerror.h>
#include <qhash.h>

class NewBookWriter
{
public:
    NewBookWriter();
    void createNewBook();
    QString bookPath();

    /// Begins a transaction on the database
    void startReading();

    /// Commits a transaction to the database
    void endReading();
    void addPage(const QString &text, int pageID, int pageNum, int partNum);
    void addTitle(const QString &title, int tid, int level);
    QString serializeBookInfo();

protected:
    void createBookTables();
    /**
      Serialize this book info
      */
    void createFileInfo();

protected:
    QString m_tempFolder;
    QString m_bookPath;
    QString m_bookInfo; ///< Save Serialization of this book info
    QSqlDatabase m_bookDB;
    QSqlQuery m_bookQuery;
    int m_pageId;
    int m_prevID;
    int m_lastLevel;
    int m_titleID;
    QHash<int, int> m_idsHash; ///< For mapping between shamela ids and our ids
    QHash<int, int> m_levels;
};

#endif // NEWBOOKWRITER_H
