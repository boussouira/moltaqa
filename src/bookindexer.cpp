#include "bookindexer.h"
#include "bookexception.h"
#include "clconstants.h"
#include "mainwindow.h"
#include "clheader.h"
#include "clutils.h"
#include "simplebookindexer.h"
#include "quranbookindexer.h"
#include "librarybookmanager.h"
#include <exception>

BookIndexer::BookIndexer(QObject *parent) :
    QThread(parent),
    m_writer(0),
    m_trackerIter(0),
    m_stop(false)
{
    m_bookManager = LibraryManager::instance()->bookManager();
}

void BookIndexer::setWirter(IndexWriter *writer)
{
    m_writer = writer;
}

void BookIndexer::setTaskIter(IndexTaskIter *iter)
{
    m_trackerIter = iter;
}

void BookIndexer::stop()
{
    m_stop = true;
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
            task->book = m_bookManager->getLibraryBook(task->bookID);

            if(!task->book)
                throw BookException(QString("No book with id %1").arg(task->bookID));

            if(task->book) {
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

                emit taskDone(task);
            }
        } catch (BookException &e) {
            qCritical() << "Indexing error:" << e.what();
        } catch (std::exception &e) {
            qCritical() << "Indexing std error:" << e.what();
        }

        task = m_trackerIter->next();
    }

    emit doneIndexing();
}

void BookIndexer::indexBook(IndexTask *task)
{
    TextBookIndexer *indexer = 0;
    if(task->book->isNormal() || task->book->isTafessir())
        indexer = new SimpleBookIndexer();
    else if (task->book->isQuran())
        indexer = new QuranBookIndexer();
    else
        throw BookException("Unknow book type", QString("Type: %1").arg(task->book->bookType));

    indexer->setLibraryBook(task->book);
    indexer->setIndexWriter(m_writer);

    indexer->open();
    indexer->start();

    delete indexer;
}

void BookIndexer::deleteBook(IndexTask *task)
{
    try {
        Term *term = new Term(BOOK_ID_FIELD, Utils::intToWChar(task->bookID));
        m_writer->deleteDocuments(term);
    }
    catch(CLuceneError &err) {
        qCritical("removeBook: CLucene Error: %s", err.what());
    }
    catch(std::exception &err){
        qCritical("removeBook: STD error: %s", err.what());
    }
    catch(...){
        qCritical("removeBook: Unkonw error");
    }
}

void BookIndexer::updateBook(IndexTask *task)
{
    deleteBook(task);
    indexBook(task);
}
