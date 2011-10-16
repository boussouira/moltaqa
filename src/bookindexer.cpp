#include "bookindexer.h"
#include "bookexception.h"

BookIndexer::BookIndexer(QObject *parent) :
    QThread(parent),
    m_writer(0),
    m_trackerIter(0),
    m_stop(false)
{
}

void BookIndexer::setWirter(IndexWriter *writer)
{
    m_writer = writer;
}

void BookIndexer::setTaskIter(IndexTaskIter *iter)
{
    m_trackerIter = iter;
}

void BookIndexer::run()
{
    startIndexing();
}

void BookIndexer::startIndexing()
{
    IndexTask *task = m_trackerIter->next();

    while(task && !m_stop) {
        try {
            indexBook(task->book);
        } catch (BookException &e) {
            qCritical() << "Indexing error:" << e.what();
        }
    }

    emit doneIndexing();
}

void BookIndexer::indexBook(LibraryBook *book)
{
    TextBookReader reader;
    reader.setBookInfo(book);

    reader.openBookDB();

    while (reader.hasNext()) {
        reader.nextPage();

    }

    emit bookIndexed(book->bookDisplayName);
}
