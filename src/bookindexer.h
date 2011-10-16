#ifndef BOOKINDEXER_H
#define BOOKINDEXER_H

#include <qthread.h>
#include "clheader.h"
#include "indextaskiter.h"
#include "textbookreader.h"

class BookIndexer : public QThread
{
    Q_OBJECT
public:
    BookIndexer(QObject *parent = 0);

    void setWirter(IndexWriter* writer);
    void setTaskIter(IndexTaskIter *iter);
    void run();

protected:
    void startIndexing();
    void indexBook(LibraryBook *book);

signals:
    void doneIndexing();
    void bookIndexed(const QString &bookName);

protected:
    IndexWriter *m_writer;
    IndexTaskIter *m_trackerIter;
    bool m_stop;
};

#endif // BOOKINDEXER_H
