#ifndef NEWQURANWRITER_H
#define NEWQURANWRITER_H

#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsqlerror.h>
#include <qhash.h>
#include <qapplication.h>

class newQuranWriter
{
    Q_DECLARE_TR_FUNCTIONS(newQuranWriter)

public:
    newQuranWriter();
    void setThreadID(int id) { m_threadID = id; }
    void createNewBook(QString bookPath=QString());
    QString bookPath();

    /// Begins a transaction on the database
    void startReading();

    /// Commits a transaction to the database
    void endReading();
    void addPage(const QString &text, int soraNum, int ayaNum, int pageNum);
    void addSoraInfo(const QString &name, const QString &SoraDescent, int ayatCount);
    void addSowarInfo();

protected:
    void createBookTables();

protected:
    QString m_tempFolder;
    QString m_bookPath;
    QSqlDatabase m_bookDB;
    QSqlQuery m_bookQuery;
    int m_threadID;
};

#endif // NEWQURANWRITER_H
