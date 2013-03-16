#include "bookindexer.h"
#include "bookexception.h"
#include "clconstants.h"
#include "mainwindow.h"
#include "clheader.h"
#include "clutils.h"
#include "bookindexersimple.h"
#include "bookindexerquran.h"
#include "librarybookmanager.h"
#include "librarymanager.h"
#include "utils.h"

#include <exception>

BookIndexerThread::BookIndexerThread(QObject *parent) :
    QThread(parent),
    m_writer(0),
    m_trackerIter(0),
    m_indexer(0),
    m_stop(false)
{
    m_bookManager = LibraryManager::instance()->bookManager();
}

void BookIndexerThread::setWirter(IndexWriter *writer)
{
    m_writer = writer;
}

void BookIndexerThread::setTaskIter(IndexTaskIter *iter)
{
    m_trackerIter = iter;
}

void BookIndexerThread::stop()
{
    m_stop = true;

    if(m_indexer)
        m_indexer->stop();
}

void BookIndexerThread::run()
{
    startIndexing();
}

void BookIndexerThread::startIndexing()
{
    IndexTask *task = m_trackerIter->next();

    while(task && !m_stop) {
        try {
            task->book = m_bookManager->getLibraryBook(task->bookID);

            if(!task->book && task->task != IndexTask::Delete)
                throw BookException(QString("No book with id %1").arg(task->bookID));

            switch (task->task) {
            case IndexTask::Add:
                indexBook(task);
                break;

            case IndexTask::Delete:
                deleteBook(task);
                break;

            case IndexTask::Update:
                updateBook(task);
                break;

            default:
                qWarning("BookIndexer: Unknow task");
                break;
            }

        } catch (BookException &e) {
            qCritical() << "BookIndexer: Indexing error:" << e.what();
        } catch (std::exception &e) {
            qCritical() << "BookIndexer: Indexing std error:" << e.what();
        }

        if(!(m_stop && task->task == IndexTask::Update))
            emit taskDone(task);

        task = m_trackerIter->next();
    }

    emit doneIndexing();
}

void BookIndexerThread::indexBook(IndexTask *task)
{
    m_indexer = 0;
    if(task->book->isNormal() || task->book->isTafessir())
        m_indexer = new BookIndexerSimple();
    else if (task->book->isQuran())
        m_indexer = new BookIndexerQuran();
    else
        throw BookException("Unknow book type", QString("Type: %1").arg(task->book->type));

    m_indexer->setLibraryBook(task->book);
    m_indexer->setIndexWriter(m_writer);

    try {
        m_indexer->open();
        m_indexer->start();
    } catch(CLuceneError &err) {
        qCritical("BookIndexer::indexBook CLucene Error: %s", err.what());
    } catch(std::exception &err) {
        qCritical("BookIndexer::indexBook STD error: %s", err.what());
    } catch(...) {
        qCritical("BookIndexer::indexBook Unkonw error");
    }

    ml_delete_check(m_indexer);
}

void BookIndexerThread::deleteBook(IndexTask *task)
{
    try {
        Term *term = new Term(BOOK_ID_FIELD, Utils::CLucene::intToWChar(task->bookID));
        m_writer->deleteDocuments(term);
    }
    catch(CLuceneError &err) {
        qCritical("BookIndexer::deleteBook CLucene Error: %s", err.what());
    }
    catch(std::exception &err){
        qCritical("BookIndexer::deleteBook STD error: %s", err.what());
    }
    catch(...){
        qCritical("BookIndexer::deleteBook Unkonw error");
    }
}

void BookIndexerThread::updateBook(IndexTask *task)
{
    deleteBook(task);
    indexBook(task);
}
