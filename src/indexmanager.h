#ifndef INDEXMANAGER_H
#define INDEXMANAGER_H

#include <QObject>
#include <qdatetime.h>
#include "bookindexer.h"
#include "clheader.h"
#include "libraryinfo.h"
#include "indextracker.h"

class ArabicAnalyzer;

class IndexManager : public QObject
{
    Q_OBJECT
public:
    IndexManager(QObject *parent = 0);

    bool isIndexing();

protected:
    bool openWriter();

public slots:
    void start();
    void stop();
    void taskDone(IndexTask *task);
    void threadDoneIndexing();

signals:
    void started();
    void progress(int value, int max);
    void taskDone(QString name);
    void done();

protected:
    LibraryInfo *m_library;
    IndexWriter *m_writer;
    IndexTracker *m_indexTracker;
    IndexTaskIter *m_taskIter;
    QList<BookIndexer*> m_threads;
    ArabicAnalyzer *m_analyzer;
    QTime m_indexingTime;
    int m_booksCount;
    int m_indexedBooks;
    int m_threadCount;
    int m_indexedBookCount;
};

#endif // INDEXMANAGER_H
