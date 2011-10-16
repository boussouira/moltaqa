#ifndef LIBRARYINDEXMANAGER_H
#define LIBRARYINDEXMANAGER_H

#include <QObject>
#include "bookindexer.h"
#include "clheader.h"
#include "libraryinfo.h"
#include "indextracker.h"

class LibraryIndexManager : public QObject
{
    Q_OBJECT
public:
    LibraryIndexManager(QObject *parent = 0);

protected:
    bool openWriter();

public slots:
    void start();
    void stop();

signals:
    void started();
    void progress(int value, int max);
    void bookIndexed(QString name);
    void done();

protected:
    LibraryInfo *m_library;
    IndexWriter *m_writer;
    IndexTracker *m_indexTracker;
    int m_booksCount;
    int m_indexedBooks;
    int m_threadCount;
};

#endif // LIBRARYINDEXMANAGER_H
