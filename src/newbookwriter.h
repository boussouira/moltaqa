#ifndef NEWBOOKWRITER_H
#define NEWBOOKWRITER_H

#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsqlerror.h>
#include <qhash.h>
#include <QTime>

class NewBookWriter
{
public:
    NewBookWriter();
    void setThreadID(int id) { m_threadID = id; }
    void createNewBook(QString bookPath=QString());
    QString bookPath();

    /// Begins a transaction on the database
    void startReading();

    /// Commits a transaction to the database
    void endReading();
    int addPage(const QString &text, int pageID, int pageNum, int partNum);
    int addPage(const QString &text, int pageID, int pageNum, int partNum, int ayaNum, int soraNum);
    void addHaddithNumber(int page_id, int hno);
    void addTitle(const QString &title, int tid, int level);

protected:
    void createBookTables();

protected:
    QString m_tempFolder;
    QString m_bookPath;
    QSqlDatabase m_bookDB;
    QSqlQuery m_bookQuery;
    int m_pageId;
    int m_prevID;
    int m_lastLevel;
    int m_titleID;
    // Book information
    QHash<int, int> m_firstPage;
    QHash<int, int> m_lastPage;

    QHash<int, int> m_idsHash; ///< For mapping between shamela ids and our ids
    QHash<int, int> m_levels;
    QTime m_time;
    int m_threadID;
    bool m_isTafessir;
};

#endif // NEWBOOKWRITER_H
