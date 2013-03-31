#ifndef BOOKINDEXER_H
#define BOOKINDEXER_H

#include <qthread.h>

class LibraryBookManager;
class BookIndexerBase;
class IndexTaskIter;
class IndexTask;

namespace lucene {
    namespace index {
        class IndexWriter;
    }
}

class BookIndexerThread : public QThread
{
    Q_OBJECT
public:
    BookIndexerThread(QObject *parent = 0);

    void setWirter(lucene::index::IndexWriter* writer);
    void setTaskIter(IndexTaskIter *iter);
    void stop();
    void run();

protected:
    void startIndexing();
    void indexBook(IndexTask *task);
    void deleteBook(IndexTask *task);
    void updateBook(IndexTask *task);

signals:
    void doneIndexing();
    void taskDone(IndexTask *task);

protected:
    lucene::index::IndexWriter *m_writer;
    IndexTaskIter *m_trackerIter;
    LibraryBookManager *m_bookManager;
    BookIndexerBase *m_indexer;
    bool m_stop;
};

#endif // BOOKINDEXER_H
