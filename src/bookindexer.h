#ifndef BOOKINDEXER_H
#define BOOKINDEXER_H

#include <qthread.h>
#include "indextaskiter.h"
#include "textbookreader.h"

namespace lucene {
    namespace index {
        class IndexWriter;
    }
}

class BookIndexer : public QThread
{
    Q_OBJECT
public:
    BookIndexer(QObject *parent = 0);

    void setWirter(lucene::index::IndexWriter* writer);
    void setTaskIter(IndexTaskIter *iter);
    void stop();
    void run();

protected:
    void startIndexing();
    void indexBook(IndexTask *task);
    void deleteBook(IndexTask *task);
    void updateBook(IndexTask *task);

    void indexQuran(IndexTask *task);
    void indexSimpleBook(IndexTask *task);
    void indexTaffesirBook(IndexTask *task);

signals:
    void doneIndexing();
    void taskDone(IndexTask *task);

protected:
    lucene::index::IndexWriter *m_writer;
    IndexTaskIter *m_trackerIter;
    bool m_stop;
};

#endif // BOOKINDEXER_H
